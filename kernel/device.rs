///! Driver model.

use alloc::rc::Rc;
use resource::{NAMESPACE, Resource};

/// Register a device to the kernel.
pub fn register_device(device: Rc<Resource>) {
    unsafe {
        NAMESPACE.add(device);
    }
}
