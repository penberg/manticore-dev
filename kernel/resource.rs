use alloc::rc::Rc;
use alloc::vec::Vec;
use core::cell::RefCell;
use event::EventListener;
use intrusive_collections::{KeyAdapter, RBTree, RBTreeLink};
use ioqueue::IOCmd;
use vm::VMAddressSpace;

// Keep this up-to-date with include/uapi/manticore/config_abi.h.
pub const CONFIG_ETHERNET_MAC_ADDRESS: i32 = 0;

/// A resource descriptor.
pub struct ResourceDesc(i32);

/// A resource configuration option.
pub type ConfigOption = i32;

impl ResourceDesc {
    pub fn from_user(desc: i32) -> ResourceDesc {
        ResourceDesc(desc)
    }

    pub fn to_user(&self) -> i32 {
        self.0
    }

    pub fn to_idx(&self) -> Option<usize> {
        if self.0 >= 0 {
            Some(self.0 as usize)
        } else {
            None
        }
    }
}

pub trait ResourceOps {
    fn subscribe(&self, vmspace: &mut VMAddressSpace, listener: Rc<dyn EventListener>);
    fn io_submit(&self, cmd: IOCmd);
    fn get_config(&self, option: ConfigOption) -> Option<Vec<u8>>;
}

pub struct Resource {
    name: &'static str,
    ops: RefCell<Rc<dyn ResourceOps>>,
    link: RBTreeLink,
}

intrusive_adapter!(pub ResourceAdapter = Rc<Resource>: Resource { link: RBTreeLink });

impl Resource {
    pub fn new(name: &'static str, ops: RefCell<Rc<dyn ResourceOps>>) -> Self {
        Resource {
            name: name,
            ops: ops,
            link: RBTreeLink::new(),
        }
    }

    pub fn subscribe(&self, vmspace: &mut VMAddressSpace, listener: Rc<dyn EventListener>) {
        self.ops.borrow().subscribe(vmspace, listener);
    }

    pub fn get_config(&self, option: ConfigOption) -> Option<Vec<u8>> {
        return self.ops.borrow().get_config(option);
    }
}

impl<'a> KeyAdapter<'a> for ResourceAdapter {
    type Key = &'static str;

    fn get_key(&self, x: &'a Resource) -> Self::Key {
        x.name
    }
}

pub struct Namespace {
    resources: RBTree<ResourceAdapter>,
}

impl Namespace {
    pub const fn new() -> Self {
        Namespace {
            resources: RBTree::new(ResourceAdapter::NEW),
        }
    }

    pub fn add(&mut self, resource: Rc<Resource>) {
        self.resources.insert(resource);
    }

    pub fn lookup(&self, name: &'static str) -> Option<Rc<Resource>> {
        let cursor = self.resources.find(name);
        return cursor.clone_pointer();
    }
}

pub static mut NAMESPACE: Namespace = Namespace::new();

pub struct ResourceSpace {
    desc_table: Vec<Rc<Resource>>,
}

impl ResourceSpace {
    pub fn new() -> Self {
        ResourceSpace { desc_table: vec![] }
    }

    pub fn attach(&mut self, resource: Rc<Resource>) -> ResourceDesc {
        let idx = self.desc_table.len();
        self.desc_table.push(resource);
        return ResourceDesc::from_user(idx as i32);
    }

    pub fn lookup(&self, desc: ResourceDesc) -> Option<Rc<Resource>> {
        if let Some(idx) = desc.to_idx() {
            if idx > self.desc_table.len() {
                return None;
            }
            return Some(self.desc_table[idx].clone());
        }
        return None;
    }
}
