//! Virtio network device driver.

use alloc::boxed::Box;
use intrusive_collections::LinkedList;
use kernel::device::{Device, DeviceOps};
use kernel::memory;
use kernel::mmu;
use kernel::print;
use pci::{DeviceID, PCIDevice, PCIDriver, PCI_CAPABILITY_VENDOR, PCI_VENDOR_ID_REDHAT};
use virtqueue;
use core::intrinsics::transmute;

const PCI_DEVICE_ID_VIRTIO_NET: u16 = 0x1041;

#[allow(dead_code)]
const DEVICE_FEATURE_SELECT: usize = 0x00;
#[allow(dead_code)]
const DEVICE_FEATURE: usize = 0x04;
#[allow(dead_code)]
const DRIVER_FEATURE_SELECT: usize = 0x08;
#[allow(dead_code)]
const DRIVER_FEATURE: usize = 0x0c;
#[allow(dead_code)]
const MSIX_CONFIG: usize = 0x10;
#[allow(dead_code)]
const NUM_QUEUES: usize = 0x12;
#[allow(dead_code)]
const DEVICE_STATUS: usize = 0x14;
#[allow(dead_code)]
const CONFIG_GENERATION: usize = 0x15;
#[allow(dead_code)]
const QUEUE_SELECT: usize = 0x16;
#[allow(dead_code)]
const QUEUE_SIZE: usize = 0x18;
#[allow(dead_code)]
const QUEUE_MSIX_VECTOR: usize = 0x1a;
#[allow(dead_code)]
const QUEUE_ENABLE: usize = 0x1c;
#[allow(dead_code)]
const QUEUE_NOTIFY_OFF: usize = 0x1e;
#[allow(dead_code)]
const QUEUE_DESC: usize = 0x20;
#[allow(dead_code)]
const QUEUE_AVAIL: usize = 0x28;
#[allow(dead_code)]
const QUEUE_USED: usize = 0x30;

#[allow(dead_code)]
const VIRTIO_ACKNOWLEDGE: u8 = 1;
#[allow(dead_code)]
const VIRTIO_DRIVER: u8 = 2;
#[allow(dead_code)]
const VIRTIO_FAILED: u8 = 128;
#[allow(dead_code)]
const VIRTIO_FEATURES_OK: u8 = 8;
#[allow(dead_code)]
const VIRTIO_DRIVER_OK: u8 = 4;
#[allow(dead_code)]
const DEVICE_NEEDS_RESET: u8 = 64;

bitflags! {
    pub struct Features: u32 {
        const VIRTIO_NET_F_CSUM = 1<<0;
        const VIRTIO_NET_F_GUEST_CSUM = 1<<1;
        const VIRTIO_NET_F_CTRL_GUEST_OFFLOADS = 1<<2;
        const VIRTIO_NET_F_MAC = 1<<5;
        const VIRTIO_NET_F_GUEST_TSO4 = 1<<7;
        const VIRTIO_NET_F_GUEST_TSO6 = 1<<8;
        const VIRTIO_NET_F_GUEST_ECN = 1<<9;
        const VIRTIO_NET_F_GUEST_UFO = 1<<10;
        const VIRTIO_NET_F_HOST_TSO4 = 1<<11;
        const VIRTIO_NET_F_HOST_TSO6 = 1<<12;
        const VIRTIO_NET_F_HOST_ECN = 1<<13;
        const VIRTIO_NET_F_HOST_UFO = 1<<14;
        const VIRTIO_NET_F_MRG_RXBUF = 1<<15;
        const VIRTIO_NET_F_STATUS = 1<<16;
        const VIRTIO_NET_F_CTRL_VQ = 1<<17;
        const VIRTIO_NET_F_CTRL_RX = 1<<18;
        const VIRTIO_NET_F_CTRL_VLAN = 1<<19;
        const VIRTIO_NET_F_GUEST_ANNOUNCE = 1<<21;
        const VIRTIO_NET_F_MQ = 1<<22;
        const VIRTIO_NET_F_CTRL_MAC_ADDR = 1<<23;
    }
}

#[allow(dead_code)]
const VIRTIO_PCI_CAP_COMMON_CFG: u8 = 1;
#[allow(dead_code)]
const VIRTIO_PCI_CAP_NOTIFY_CFG: u8 = 2;
#[allow(dead_code)]
const VIRTIO_PCI_CAP_ISR_CFG: u8 = 3;
#[allow(dead_code)]
const VIRTIO_PCI_CAP_DEVICE_CFG: u8 = 4;
#[allow(dead_code)]
const VIRTIO_PCI_CAP_PCI_CFG: u8 = 5;

const VIRTIO_PCI_CAP_CFG_TYPE: u8 = 3;
const VIRTIO_PCI_CAP_BAR: u8 = 4;

struct VirtioNetDevice {
    vqs: LinkedList<virtqueue::VirtqueueAdapter>,
}

impl VirtioNetDevice {
    fn probe(pci_dev: &PCIDevice) -> Device {
        pci_dev.set_bus_master(true);

        pci_dev.enable_msix();

        let mut dev = VirtioNetDevice::new();

        let bar_idx = VirtioNetDevice::find_capability(pci_dev, VIRTIO_PCI_CAP_DEVICE_CFG);

        println!(
            "virtio-net: using PCI BAR{} for device configuration",
            bar_idx.unwrap()
        );

        let ioport = pci_dev.bars[bar_idx.unwrap()].clone().unwrap();

        //
        // 1. Reset device
        //
        let mut status: u8 = 0;
        unsafe { ioport.write8(status, DEVICE_STATUS) };

        //
        // 2. Set ACKNOWLEDGE status bit
        //
        status |= VIRTIO_ACKNOWLEDGE;
        unsafe { ioport.write8(status, DEVICE_STATUS) };

        //
        // 3. Set DRIVER status bit
        //
        status |= VIRTIO_DRIVER;
        unsafe { ioport.write8(status, DEVICE_STATUS) };

        //
        // 4. Negotiate features
        //
        unsafe { ioport.write32(VIRTIO_NET_F_MRG_RXBUF.bits(), DRIVER_FEATURE) };

        //
        // 5. Set the FEATURES_OK status bit
        //
        status |= VIRTIO_FEATURES_OK;
        unsafe { ioport.write8(status, DEVICE_STATUS) };

        //
        // 6. Re-read device status to ensure the FEATURES_OK bit is still set
        //
        if unsafe { ioport.read8(DEVICE_STATUS) } & VIRTIO_FEATURES_OK != VIRTIO_FEATURES_OK {
            panic!("Device does not support our subset of features");
        }

        //
        // 7. Perform device-specific setup
        //
        let num_queues = unsafe { ioport.read16(NUM_QUEUES) };

        println!("virtio-net: {} virtqueues found.", num_queues);

        for queue in 0..num_queues {
            unsafe { ioport.write16(queue as u16, QUEUE_SELECT) };

            let size = unsafe { ioport.read16(QUEUE_SIZE) };

            let vq = Box::new(virtqueue::Virtqueue::new(size as usize));

            unsafe {
                ioport.write64(
                    mmu::virt_to_phys(vq.raw_descriptor_table_ptr) as u64,
                    QUEUE_DESC,
                );
                ioport.write64(
                    mmu::virt_to_phys(vq.raw_available_ring_ptr) as u64,
                    QUEUE_AVAIL,
                );
                ioport.write64(mmu::virt_to_phys(vq.raw_used_ring_ptr) as u64, QUEUE_USED);

                // RX queue:
                if queue == 0 {
                    // FIXME: Clean this up!
                    let page = memory::page_alloc_small();
                    // FIXME: Store raw pointer separately!
                    vq.add_inbuf(mmu::virt_to_phys(page as usize) as usize, 4096);
                    let vector = pci_dev.register_irq(queue, VirtioNetDevice::interrupt, transmute(&*vq));
                    pci_dev.enable_irq(queue);
                    ioport.write16(queue, QUEUE_MSIX_VECTOR);
                    println!(
                        "virtio-net: virtqueue {} is using IRQ vector {}",
                        queue, vector
                    );
                }
                ioport.write16(1 as u16, QUEUE_ENABLE);
            }

            dev.add_vq(vq);
        }

        //
        // 8. Set DRIVER_OK status bit
        //
        status |= VIRTIO_DRIVER_OK;
        unsafe {
            ioport.write8(status, DEVICE_STATUS);
        }

        Device::new(Box::new(dev))
    }

    fn find_capability(pci_dev: &PCIDevice, cfg_type: u8) -> Option<usize> {
        let mut capability = pci_dev.func.find_capability(PCI_CAPABILITY_VENDOR);
        while let Some(offset) = capability {
            let ty = pci_dev
                .func
                .read_config_u8(offset + VIRTIO_PCI_CAP_CFG_TYPE);
            let bar = pci_dev.func.read_config_u8(offset + VIRTIO_PCI_CAP_BAR);
            if ty == cfg_type && bar < 0x05 {
                return Some(bar as usize);
            }
            capability = pci_dev
                .func
                .find_next_capability(PCI_CAPABILITY_VENDOR, offset);
        }
        None
    }

    fn recv(&self) {
        for ref mut vq in self.vqs.iter() {
            unsafe {
                // FIXME: Used ring is little endian!
                let last_idx = (*vq.used_ring()).idx;
                if vq.last_seen_used() != last_idx {
                    let idx = vq.last_seen_used(); // FIXME: iterate over a range!
                    let used = &(*vq.used_ring()).ring[idx as usize];
                    let start = vq.get_buf(used.id as u16) as u64;
                    /*
                    let ptr0: *const u8 = mmu::phys_to_virt((start + 12) as usize) as *const u8;
                    let hdr: *const VirtioNetHdr = mem::transmute(ptr0);
                    let ptr: *const u8 = mmu::phys_to_virt((start + 12) as usize) as *const u8;
                    let data = unsafe { slice::from_raw_parts(ptr0, used.len as usize) };
                    let ethernet: *const Ethernet = mem::transmute(ptr);
                    match (*ethernet).ether_type() {
                        EtherType::Arp => {
                            let arp: *const ArpHeader = mem::transmute(ptr);
                            println!("{:?}", *arp)
                        }
                        _ => println!("{:?} {:?}", *hdr, *ethernet)
                    }
                    */
                    vq.advance_last_seen_used();
                    let avail = vq.available_ring();
                    (*avail).ring[((*avail).idx % vq.queue_size as u16) as usize] = 0;
                    (*avail).idx += 1;
                }
            }
        }
    }

    extern "C" fn interrupt(arg: usize) {
        let mut vq_p: *mut virtqueue::Virtqueue = unsafe { transmute(arg) };
        let mut vq = unsafe { &*vq_p };
        println!("virtio-net interrupt: {:?}", vq);
        /* FIXME: call recv */
        let last_idx = unsafe { (*vq.used_ring()).idx };
        if vq.last_seen_used() != last_idx {
            println!("virtqueue has data");
            vq.advance_last_seen_used();
            let avail = vq.available_ring();
            unsafe { (*avail).ring[((*avail).idx % vq.queue_size as u16) as usize] = 0; }
            unsafe { (*avail).idx += 1; }
        }
    }

    fn new() -> Self {
        VirtioNetDevice {
            vqs: LinkedList::new(virtqueue::VirtqueueAdapter::new()),
        }
    }

    fn add_vq(&mut self, vq: Box<virtqueue::Virtqueue>) {
        self.vqs.push_back(vq);
    }
}

impl DeviceOps for VirtioNetDevice {}

pub static mut VIRTIO_NET_DRIVER: PCIDriver = PCIDriver::new(
    DeviceID::new(PCI_VENDOR_ID_REDHAT, PCI_DEVICE_ID_VIRTIO_NET),
    VirtioNetDevice::probe,
);
