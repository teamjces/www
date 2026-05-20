// SPDX-License-Identifier: CC0-1.0
// This file is released into the public domain.
// See https://creativecommons.org/publicdomain/zero/1.0/

use std::cell::RefCell;
use std::time::{SystemTime, UNIX_EPOCH};

thread_local! {
    static STATE: RefCell<u32> = RefCell::new(seed());
}

fn seed() -> u32 {
    let nanos = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap()
        .as_nanos();
    let tmp = (nanos & 0xFFFF_FFFF) as u32;
    if tmp == 0 { 1 } else { tmp }
}

pub fn srand() {
    STATE.with(|s| *s.borrow_mut() = seed());
}

pub fn rand(range: u32) -> u32 {
    STATE.with(|s| {
        let mut val = s.borrow_mut();
        let next = ((*val as u64 * 48271) % 0x7FFFFFFF) as u32;
        *val = next;
        next % range
    })
}
