//!
//! Kernel services.
//!

#![feature(global_allocator)]
#![feature(allocator_api)]
#![feature(box_syntax)]
#![feature(const_fn)]
#![feature(alloc)]
#![feature(asm)]
#![no_std]

#[macro_use]
extern crate intrusive_collections;
extern crate xmas_elf;
extern crate alloc;
extern crate rlibc;

#[macro_use]
pub mod print;
pub mod memory;
pub mod mmu;
mod elf;

pub use memory::memory_add_span;
pub use memory::page_alloc_init;
pub use memory::page_alloc_small;
pub use memory::page_free_small;
pub use memory::page_alloc_large;
pub use memory::page_free_large;
pub use elf::parse_elf;

use memory::KAllocator;

#[global_allocator]
static mut ALLOCATOR: KAllocator = KAllocator::new();
