Emulated system memory and address spaces management
====================================================

1. Overview
-----------

The memory subsystem (emumem and addrmap) combines multiple functions
useful for system emulation:
* address bus decoding and dispatching with caching
* static descriptions of an address map
* ram allocation and registration for state saving
* interaction with memory regions to access rom

Devices create address spaces, e.g. decodable buses, through the
device_memory_interface.  The machine configuration sets up address
maps to put in the address spaces, then the device can do read and
writes through the bus.

2. Basic concepts
-----------------

2.1 Address spaces
~~~~~~~~~~~~~~~~~~

An address space, implemented in the class **address_space**,
represents an addressable bus with potentially multiple sub-devices
connected requiring a decode.  It has a number of data lines (8, 16,
32 or 64) called data width, a number of address lines (1 to 32)
called address width and an endianness.  In addition an address shift
allows for buses that have an atomic granularity different than a
byte.

Address space objects provide a series of methods for read and write
access, and a second series of methods for dynamically changing the
decode.


2.2 Address maps
~~~~~~~~~~~~~~~~

An address map is a static description of the decode expected when
using a bus.  It connects to memory, other devices and methods, and is
installed, usually at startup, in an address space.  That description
is stored in an **address_map** structure which is filled
programatically.


2.3 Shares, banks and regions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Memory shares are allocated memory zones that can be put in multiple
places in the same or different address spaces, and can also be
directly accessed from devices.

Memory banks are zones that indirect memory access, giving the
possibility to dynamically and efficiently change where a zone
actually points to.

Memory regions are read-only memory zones in which roms are loaded.


3. Address maps API
-------------------

3.1 General API structure
~~~~~~~~~~~~~~~~~~~~~~~~~

A memory is a method of a device which fills an **address_map**
structure, usually called **map**, passed by reference.  The method
then can set some global configuration through specific methods and
then provide address range-oriented entries which indicate what should
happen when a specific range is accessed.

The general syntax for entries uses method chaining:

| map(start, end).handler(...).handler_qualifier(...).range_qualifier();

The values start and end define the range, the handler() block defines
how the access is handled, the handler_qualifier() block specifies
some aspects of the handler (memory sharing for instance) and the
range_qualifier() block refines the range (mirroring, masking, byte
selection...).

3.2 Global configurations
~~~~~~~~~~~~~~~~~~~~~~~~~

3.2.1 Global masking
''''''''''''''''''''

| map.global_mask(offs_t mask);

Allows to indicates a mask to be applied to all addresses when
accessing the space that map is installed in.


3.2.2 Returned value on unmapped/nop-ed read
''''''''''''''''''''''''''''''''''''''''''''

| map.unmap_value_low();
| map.unmap_value_high();
| map.unmap_value(u8 value);

Sets the value to return on reads to an unmapped or nopped-out
address.  Low means 0, high ~0.

3.3 Handler setting
~~~~~~~~~~~~~~~~~~~

3.3.1 Method on the current device
''''''''''''''''''''''''''''''''''

| (...).r(FUNC(my_device::read_method))
| (...).w(FUNC(my_device::write_method))
| (...).rw(FUNC(my_device::read_method), FUNC(my_device::write_method))
|
| uNN my_device::read_method(address_space &space, offs_t offset, uNN mem_mask)
| uNN my_device::read_method(address_space &space, offs_t offset)
| uNN my_device::read_method(address_space &space)
| uNN my_device::read_method(offs_t offset, uNN mem_mask)
| uNN my_device::read_method(offs_t offset)
| uNN my_device::read_method()
|
| void my_device::write_method(address_space &space, offs_t offset, uNN data, uNN mem_mask)
| void my_device::write_method(address_space &space, offs_t offset, uNN data)
| void my_device::write_method(address_space &space, uNN data)
| void my_device::write_method(offs_t offset, uNN data, uNN mem_mask)
| void my_device::write_method(offs_t offset, uNN data)
| void my_device::write_method(uNN data)

Sets a method of the current device to read, write or both for the
current entry.  The prototype of the method can take multiple forms
making some elements optional.  uNN represents u8, u16, u32 or u64
depending on the data width of the handler.  The handler can be less
wide than the bus itself (for instance a 8-bits device on a 32-bits
bus).

3.3.2 Method on a different device
''''''''''''''''''''''''''''''''''

| (...).r(m_other_device, FUNC(other_device::read_method))
| (...).r("other-device-tag", FUNC(other_device::read_method))
| (...).w(m_other_device, FUNC(other_device::write_method))
| (...).w("other-device-tag", FUNC(other_device::write_method))
| (...).rw(m_other_device, FUNC(other_device::read_method), FUNC(other_device::write_method))
| (...).rw("other-device-tag", FUNC(other_device::read_method), FUNC(other_device::write_method))

Sets a method of another device, designated by a finder
(required_device or optional_device) or its tag, to read, write or
both for the current entry.

3.3.3 Lambda function
'''''''''''''''''''''

| (...).lr{8,16,32,64}(FUNC([...](address_space &space, offs_t offset, uNN mem_mask) -> uNN { ... }))
| (...).lr{8,16,32,64}([...](address_space &space, offs_t offset, uNN mem_mask) -> uNN { ... }, "name")
| (...).lw{8,16,32,64}(FUNC([...](address_space &space, offs_t offset, uNN data, uNN mem_mask) -> void { ... }))
| (...).lw{8,16,32,64}([...](address_space &space, offs_t offset, uNN data, uNN mem_mask) -> void { ... }, "name")
| (...).lrw{8,16,32,64}(FUNC(read), FUNC(write))
| (...).lrw{8,16,32,64}(read, "name_r", write, "name_w")

Sets a lambda called on read, write or both.  The lambda prototype can
be any of the 6 available for methods.  One can either use FUNC() over
the whole lambda or provide a name after the lambda definition.

3.3.4 Direct memory access
''''''''''''''''''''''''''

