#include "../common/core.h"

static void n1(void) {
    out_line("- Stored program concept: instructions AND data held together in the same main memory, in binary.");
    out_line("- One CPU fetches and executes instructions one at a time (sequentially).");
    out_line("- Shared buses between CPU and memory: address bus, data bus, control bus.");
    out_line(HEAD "Von Neumann bottleneck");
    out_line("- Instructions and data share one bus, so they can't be fetched at the same time. The CPU often waits for memory.");
    out_line(HEAD "Harvard (comparison)");
    out_line("- Separate memories and buses for instructions and data, so both can be fetched at once. Used in embedded systems/DSPs.");
    out_line(ANS "Exam tip: Exam: \"stored program\" = program stored in memory alongside data and can be changed.");
}
static void n2(void) {
    out_line(HEAD "ALU");
    out_line("- Does arithmetic (+ -) and logic (AND, OR, comparisons).");
    out_line(HEAD "Control unit (CU)");
    out_line("- Decodes instructions, sends control signals, coordinates the fetch-execute cycle.");
    out_line(HEAD "Clock");
    out_line("- Sends timing pulses. Speed in Hz (e.g. 3.2 GHz = 3.2 billion cycles/s).");
    out_line(HEAD "Registers (very fast storage inside CPU)");
    out_line("- PC (program counter): address of NEXT instruction.");
    out_line("- MAR: address being read/written.");
    out_line("- MDR (MBR): data/instruction just fetched or about to be written.");
    out_line("- CIR: current instruction being decoded/executed.");
    out_line("- Accumulator (ACC): results from the ALU.");
    out_line("- Status register: flags e.g. carry, zero, negative, overflow.");
    out_line(HEAD "Buses");
    out_line("- Address bus: one way, CPU \x1e"" memory. Width sets max addressable memory (n lines = 2^n addresses).");
    out_line("- Data bus: two way. Width = bits moved at once.");
    out_line("- Control bus: two way. Read/write, clock, interrupt signals.");
}
static void n3(void) {
    out_line(HEAD "Fetch");
    out_line("- PC \x1e"" MAR (address of next instruction).");
    out_line("- Instruction at that address copied via data bus into MDR.");
    out_line("- PC incremented (PC = PC + 1).");
    out_line("- MDR \x1e"" CIR.");
    out_line(HEAD "Decode");
    out_line("- CU splits CIR into opcode + operand and works out what to do.");
    out_line(HEAD "Execute");
    out_line("- Instruction carried out, e.g. ALU does maths, result to ACC, data loaded/stored, or PC changed for a jump.");
    out_line("- Interrupts are checked at the end of each cycle.");
    out_line(ANS "Exam tip: Learn the register transfers in order: PC\x1e""MAR, [MAR]\x1e""MDR, PC+1, MDR\x1e""CIR.");
}
static void n4(void) {
    out_line(HEAD "RAM");
    out_line("- Volatile: lost when power off. Read/write. Holds OS, running programs and data.");
    out_line(HEAD "ROM");
    out_line("- Non-volatile, read only. Holds the boot program (BIOS/UEFI firmware).");
    out_line(HEAD "Cache");
    out_line("- Very fast SRAM, small, on or near the CPU.");
    out_line("- Holds frequently/recently used instructions and data so the CPU doesn't wait for slower RAM.");
    out_line("- Levels: L1 (smallest, fastest, per core), L2, L3 (largest, shared).");
    out_line("- Cache hit = found in cache; miss = fetch from RAM.");
    out_line(HEAD "Virtual memory");
    out_line("- Part of secondary storage used as extra RAM when RAM is full. Pages swapped in/out. Slow (\"disk thrashing\" if overused).");
}
static void n5(void) {
    out_line("- Clock speed: more cycles per second = more instructions per second.");
    out_line("- Number of cores: can run separate tasks in parallel (only helps if software is parallel).");
    out_line("- Cache size: bigger = more hits, fewer slow RAM accesses.");
    out_line("- Word length / bus widths: more bits processed or moved at once.");
    out_line(ANS "Exam tip: Doubling cores rarely doubles speed: some tasks can't be split, and cores share resources.");
}
static void n6(void) {
    out_line("- Using more than one processor (or core) to work on a task at the same time.");
    out_line("- SIMD: one instruction, many data items (e.g. GPU, graphics, vector maths).");
    out_line("- MIMD: many processors each running their own instructions on their own data (multicore CPUs).");
    out_line("- Pipelining: overlapping fetch/decode/execute of different instructions.");
    out_line(HEAD "Pros");
    out_line("- Faster for tasks that split up (graphics, simulations, scientific work).");
    out_line(HEAD "Cons");
    out_line("- Harder to program; some tasks are sequential; overhead coordinating processors; shared memory conflicts.");
}
static void n7(void) {
    out_line("- Choose devices by the scenario: user, environment, data type, speed.");
    out_line(HEAD "Input examples");
    out_line("- Keyboard, mouse, touchscreen, scanner, barcode/QR reader, OMR, OCR, MICR (cheques), RFID, sensors (temp, light, pressure), microphone, camera, biometric scanners.");
    out_line(HEAD "Output examples");
    out_line("- Monitor, printers (laser fast/high volume, inkjet cheap/photo, 3D), speakers, actuators/motors, LEDs, braille display.");
    out_line(HEAD "Accessibility");
    out_line("- Voice input, eye tracking, puff-suck switches, screen readers, large keyboards.");
    out_line(ANS "Exam tip: Always justify: \"barcode reader because it's fast and avoids typing errors at a till\".");
}
static void n8(void) {
    out_line("- Non-volatile, long term storage.");
    out_line(HEAD "Magnetic (HDD, tape)");
    out_line("- Cheap per GB, large capacity, moving parts, slower. Tape: sequential access, backups/archives.");
    out_line(HEAD "Optical (CD, DVD, Blu-ray)");
    out_line("- Cheap, portable, lower capacity, easily scratched. Laser reads pits/lands.");
    out_line(HEAD "Solid state (SSD, flash, SD)");
    out_line("- No moving parts, fast access, robust, low power, more expensive per GB, limited write cycles.");
    out_line(HEAD "Cloud");
    out_line("- Accessible anywhere, scalable, needs internet, security/privacy concerns.");
    out_line(ANS "Exam tip: Compare using: capacity, speed, cost, portability, durability, reliability.");
}
static void n9(void) {
    out_line("- On a hard disk, files are stored in blocks. As files are created, deleted and grow, a file's blocks get scattered (fragmented).");
    out_line("- The read head has to move around more, so access is slower.");
    out_line(HEAD "Defragmentation");
    out_line("- Utility that moves blocks so each file is stored contiguously and free space is grouped together.");
    out_line("- Faster read times.");
    out_line(ANS "Exam tip: SSDs shouldn't be defragged: no moving head, and extra writes wear them out.");
}
static void n10(void) {
    out_line("- LAN: small area (one site), owned by organisation, own cabling/WiFi.");
    out_line("- WAN: large area, connects LANs, uses third party infrastructure (telecoms).");
    out_line(HEAD "Topologies");
    out_line("- Bus: one cable, cheap, collisions, single point of failure.");
    out_line("- Star: central switch, reliable (one cable fails = one node), easy to add nodes, switch is a single point of failure.");
    out_line("- Ring: data passes around in one direction, no collisions.");
    out_line("- Mesh: many links, very reliable/redundant, expensive.");
    out_line(HEAD "Client-server vs peer-to-peer");
    out_line("- Client-server: central server manages files, security, backups. Costly, needs admin.");
    out_line("- P2P: all equal, cheap, no central control, harder backup/security.");
    out_line(HEAD "Hardware");
    out_line("- NIC, switch (sends to right device by MAC), router (between networks by IP), WAP, modem.");
}
static void n11(void) {
    out_line("- Protocol: agreed set of rules for communication (format, timing, error checking).");
    out_line(HEAD "TCP/IP stack (4 layers)");
    out_line("- Application: HTTP/HTTPS, FTP, SMTP, POP3/IMAP, DNS.");
    out_line("- Transport: TCP (splits into packets, numbers them, reliable, resends lost) / UDP (fast, no checks, streaming).");
    out_line("- Internet/Network: IP addressing and routing of packets.");
    out_line("- Link: physical transmission, MAC addresses, Ethernet/WiFi.");
    out_line(HEAD "Why layers");
    out_line("- Each layer independent, can be changed/developed separately, easier to standardise and debug.");
}
static void n12(void) {
    out_line("- Internet = global network of networks. WWW = web pages accessed over the internet (a service).");
    out_line("- IP address: unique address of a device (IPv4 32-bit, IPv6 128-bit).");
    out_line("- URL: human-readable address, e.g. https://www.wjec.co.uk/page");
    out_line("- DNS: converts domain names to IP addresses (hierarchy of DNS servers).");
    out_line("- ISP: company that connects users to the internet.");
    out_line("- HTTP/HTTPS: transfers web pages (HTTPS encrypted with TLS).");
    out_line("- Packet switching used: data split into packets, routed independently.");
}
static const calc_t c_0_0[] = {{"Von Neumann architecture", n1}, {"CPU components & registers", n2}, {"Fetch-decode-execute cycle", n3}, {"Memory: RAM, ROM, cache", n4}, {"CPU performance factors", n5}, {"Parallel processing", n6}, {"Input & output devices", n7}, {"Secondary storage", n8}, {"Disk fragmentation", n9}, {"Networks", n10}, {"Protocols & TCP/IP", n11}, {"The internet", n12}};
static void n13(void) {
    out_line(HEAD "AND (A.B)");
    out_line("- 1 only if both are 1.");
    out_line(HEAD "OR (A+B)");
    out_line("- 1 if either (or both) is 1.");
    out_line(HEAD "NOT (A-bar)");
    out_line("- Inverts.");
    out_line(HEAD "XOR (A (+) B)");
    out_line("- 1 if exactly one is 1 (inputs differ).");
    out_line(HEAD "Truth table rows");
    out_line("- n inputs \x1e"" 2^n rows. Count up in binary: 00, 01, 10, 11.");
    out_line(ANS "Exam tip: Draw the table systematically and include intermediate columns for marks.");
}
static void n14(void) {
    out_line("- Apply a logical operation with a \"mask\" to change or test certain bits.");
    out_line("- AND with 0 clears bits; AND with 1 keeps bits. e.g. 1011 0110 AND 0000 1111 = 0000 0110 (keep lower nibble).");
    out_line("- OR with 1 sets bits; OR with 0 keeps. e.g. set bit 7: OR 1000 0000.");
    out_line("- XOR with 1 toggles (flips) bits; XOR with 0 keeps.");
    out_line("- Testing a bit: AND with a mask that has 1 in that position; result non-zero = bit was set.");
    out_line(ANS "Exam tip: State which operation AND the mask value, then show the working.");
}
static void n15(void) {
    out_line(HEAD "Laws");
    out_line("- A.0 = 0, A.1 = A, A+0 = A, A+1 = 1");
    out_line("- A.A = A, A+A = A");
    out_line("- A.NOT A = 0, A+NOT A = 1");
    out_line("- NOT NOT A = A");
    out_line("- Commutative: A.B = B.A; Associative; Distributive: A.(B+C) = A.B + A.C");
    out_line("- Absorption: A + A.B = A, A.(A+B) = A");
    out_line(HEAD "Method");
    out_line("- Expand brackets, use the laws to cancel, factorise out common terms.");
    out_line("- Check with a truth table if unsure.");
    out_line(ANS "Exam tip: Write the name of the law you use on each line.");
}
static const calc_t c_0_1[] = {{"AND, OR, NOT, XOR", n13}, {"Masking", n14}, {"Boolean simplification", n15}};
static void n16(void) {
    out_line(HEAD "Serial");
    out_line("- One bit at a time down a single wire.");
    out_line("- Reliable over long distances, cheaper cables, no skew. e.g. USB, Ethernet.");
    out_line(HEAD "Parallel");
    out_line("- Several bits at once on multiple wires.");
    out_line("- Faster over short distances, but skew (bits arrive at different times) and crosstalk over long distances. e.g. internal buses.");
    out_line(ANS "Exam tip: Modern links are mostly serial because high-speed serial avoids skew.");
}
static void n17(void) {
    out_line("- Simplex: one direction only (e.g. TV broadcast, sensor to computer).");
    out_line("- Half duplex: both directions but only one at a time (e.g. walkie-talkie).");
    out_line("- Full duplex: both directions at the same time (e.g. phone call, Ethernet).");
}
static void n18(void) {
    out_line("- Combining several signals so they share one transmission medium.");
    out_line("- A multiplexer combines; a demultiplexer separates at the other end.");
    out_line("- Time division: each signal gets a time slot in turn.");
    out_line("- Frequency division: each signal uses a different frequency band.");
    out_line("- Saves cost of separate lines.");
}
static void n19(void) {
    out_line(HEAD "Circuit switching");
    out_line("- Dedicated path set up for the whole communication (old telephone network).");
    out_line("- Guaranteed bandwidth, data in order, but path reserved even when idle (wasteful).");
    out_line(HEAD "Packet switching");
    out_line("- Data split into packets; each routed independently, may take different routes; reassembled at the destination in order.");
    out_line("- Efficient use of the network, resilient (re-route around failures), but packets can arrive out of order or be delayed.");
}
static void n20(void) {
    out_line(HEAD "Header");
    out_line("- Source and destination IP address.");
    out_line("- Packet (sequence) number.");
    out_line("- Number of packets / protocol / TTL (hop limit).");
    out_line(HEAD "Payload");
    out_line("- The actual data.");
    out_line(HEAD "Trailer");
    out_line("- Error check (checksum / CRC), end of packet marker.");
}
static void n21(void) {
    out_line("- Happen on shared media when two devices transmit at the same time and signals interfere.");
    out_line(HEAD "CSMA/CD (wired)");
    out_line("- Listen before sending; if a collision is detected, stop, send a jam signal, wait a random time, retry.");
    out_line(HEAD "CSMA/CA (WiFi)");
    out_line("- Tries to avoid collisions: listen, wait, optional RTS/CTS handshake.");
    out_line("- Switches reduce collisions by giving each device its own link.");
}
static void n22(void) {
    out_line("- Routers read the destination IP in each packet header and forward it towards its destination.");
    out_line("- Routing tables store routes/next hops; updated dynamically.");
    out_line("- Each hop decrements TTL; packet dropped at 0 (stops infinite loops).");
    out_line("- Routes chosen by cost: number of hops, speed, congestion.");
}
static const calc_t c_0_2[] = {{"Serial vs parallel", n16}, {"Simplex, half & full duplex", n17}, {"Multiplexing", n18}, {"Circuit vs packet switching", n19}, {"Packet contents", n20}, {"Collisions", n21}, {"Routing", n22}};
static void n23(void) {
    out_line("- Bit: 0 or 1. Nibble: 4 bits. Byte: 8 bits.");
    out_line("- Word: the number of bits the CPU processes as one unit (e.g. 32/64 bits).");
    out_line("- Units: KB = 1000 bytes (KiB = 1024). MB, GB, TB scale by 1000 (or 1024 for binary units).");
    out_line("- n bits can represent 2^n different values.");
}
static void n24(void) {
    out_line(HEAD "Binary to denary");
    out_line("- Add the place values: 128 64 32 16 8 4 2 1. e.g. 1011 0101 = 128+32+16+4+1 = 181.");
    out_line(HEAD "Hexadecimal");
    out_line("- Base 16: 0-9 then A=10 ... F=15. One hex digit = one nibble.");
    out_line("- Binary to hex: split into nibbles. 1011 0101 = B5.");
    out_line("- Why hex: shorter, easier for humans to read, fewer errors (colour codes, MAC addresses, memory addresses).");
}
static void n25(void) {
    out_line("- Character set: every character a computer can use, each with a unique binary code.");
    out_line(HEAD "ASCII");
    out_line("- 7 bits = 128 characters (extended 8-bit = 256). English letters, digits, symbols, control codes. 'A' = 65, 'a' = 97, '0' = 48.");
    out_line(HEAD "Unicode");
    out_line("- Up to 32 bits (UTF-8/16/32). Covers all world languages, emoji. Uses more storage.");
    out_line(ANS "Exam tip: ASCII codes are in order, so you can compare/sort letters by code.");
}
static void n26(void) {
    out_line("- Integer: whole numbers (e.g. 42).");
    out_line("- Real/float: numbers with a fractional part (3.14).");
    out_line("- Boolean: TRUE/FALSE (1 bit logically, often a byte).");
    out_line("- Character: a single symbol.");
    out_line("- String: sequence of characters (strictly not primitive in all languages).");
    out_line("- Date/time in some languages.");
    out_line(ANS "Exam tip: Choose the type for a field and justify it: phone numbers are strings (leading 0, no arithmetic).");
}
static void n27(void) {
    out_line(HEAD "Addition rules");
    out_line("- 0+0=0, 0+1=1, 1+1=0 carry 1, 1+1+1=1 carry 1.");
    out_line("- Overflow: result needs more bits than available (carry out of the most significant bit in unsigned).");
    out_line(HEAD "Subtraction");
    out_line("- Add the two's complement of the number being subtracted. Ignore the final carry out.");
    out_line(HEAD "Multiplication by shifting");
    out_line("- Logical shift left one place = x2; right = /2 (for unsigned).");
}
static void n28(void) {
    out_line(HEAD "Sign and magnitude");
    out_line("- MSB is the sign (0 = +, 1 = -), rest is the magnitude. Two zeros (+0, -0); arithmetic awkward.");
    out_line(HEAD "Two's complement");
    out_line("- MSB has a negative place value (-128 for 8 bits).");
    out_line("- To negate: invert all bits then add 1 (or copy bits from the right up to and including the first 1, then flip the rest).");
    out_line("- 8-bit range: -128 to +127. One zero; normal addition works.");
    out_line("- e.g. -20: 20 = 0001 0100 \x1e"" invert 1110 1011 \x1e"" +1 = 1110 1100.");
}
static void n29(void) {
    out_line("- Number = mantissa x 2^exponent. Both stored in two's complement.");
    out_line("- More mantissa bits = more precision (accuracy). More exponent bits = bigger range.");
    out_line(HEAD "Normalised");
    out_line("- Positive: mantissa starts 0.1; negative: starts 1.0. Gives maximum precision and a unique representation.");
    out_line(HEAD "Converting to denary");
    out_line("- Work out the exponent, move the binary point that many places (right if positive), then convert.");
    out_line(ANS "Exam tip: Fixed number of bits = trade-off between range and precision.");
}
static void n30(void) {
    out_line("- Some numbers can't be stored exactly in binary (e.g. 0.1), so there are errors.");
    out_line("- Truncation: extra bits/digits simply cut off (always rounds towards zero).");
    out_line("- Rounding: to the nearest representable value.");
    out_line("- Errors build up over repeated calculations (cumulative error).");
    out_line("- Absolute error = |true - stored|; relative error = absolute / true.");
}
static const calc_t c_0_3[] = {{"Bits, bytes & words", n23}, {"Binary & hexadecimal", n24}, {"Character sets", n25}, {"Primitive data types", n26}, {"Binary arithmetic", n27}, {"Negative numbers", n28}, {"Floating point", n29}, {"Truncation & rounding", n30}};
static void n31(void) {
    out_line("- Fixed-size, ordered collection of items of the SAME data type, accessed by index.");
    out_line("- Static: size set when declared. Stored contiguously in memory.");
    out_line("- 1D: list, e.g. scores[0..9].");
    out_line("- 2D: table, e.g. grid[row][col]. Use nested loops to traverse.");
    out_line(ANS "Exam tip: Know whether your language indexes from 0 or 1, and state it.");
}
static void n32(void) {
    out_line("- Collection of related fields that can be of DIFFERENT data types, e.g. Student: name (string), age (integer), paid (Boolean).");
    out_line("- An array of records stores a table of data (like a file in memory).");
    out_line("- Access a field with dot notation: student.name.");
}
static void n33(void) {
    out_line("- Same type, fixed number of items \x1e"" array.");
    out_line("- Mixed types describing one thing \x1e"" record.");
    out_line("- Table of data \x1e"" 2D array (same type) or array of records (mixed).");
    out_line("- Size unknown/changing \x1e"" dynamic structure (list, linked list).");
    out_line(ANS "Exam tip: Justify in context: \"array of 30 integers because each class has 30 test scores\".");
}
static const calc_t c_0_4[] = {{"Arrays", n31}, {"Records", n32}, {"Choosing a data structure", n33}};
static void n34(void) {
    out_line("- File: collection of related records. Record: all data about one item. Field: one data item in a record.");
    out_line("- Key field: uniquely identifies each record.");
    out_line("- Fixed length records: easier to process and calculate positions but can waste space.");
    out_line("- Variable length: save space, harder to search/update.");
    out_line(HEAD "File design");
    out_line("- Decide fields, data types, lengths, key field, validation, organisation method.");
}
static void n35(void) {
    out_line("- Master file: main permanent data, sorted by key (e.g. customer accounts).");
    out_line("- Transaction file: changes collected over a period (e.g. today's payments).");
    out_line(HEAD "Batch update (sequential)");
    out_line("- Sort the transaction file into key order, then read both files together, apply changes, write a NEW master file.");
    out_line("- The old master + transaction file act as a backup (grandfather-father-son).");
}
static void n36(void) {
    out_line("- Serial: records in the order they were added. New records appended at the end. e.g. transaction logs.");
    out_line("- Sequential: records in key order.");
    out_line("- Access: read from the start until found (slow for one record, fine for processing all records, e.g. payroll).");
    out_line("- Good on tape and for batch processing.");
}
static void n37(void) {
    out_line("- Records stored in key order AND an index holds the key of the last record in each block/area with its address.");
    out_line("- Can process all records in order (sequential) or look up the index and jump to the block (direct).");
    out_line("- Overflow areas used when blocks fill; file needs reorganising periodically.");
    out_line("- e.g. a stock file read sequentially for reports and directly for enquiries.");
}
static void n38(void) {
    out_line("- Record location calculated from its key using a hashing algorithm; go straight to it.");
    out_line("- Very fast access to individual records. Needs direct access storage (disk).");
    out_line("- Collisions (two keys \x1e"" same address) need handling (overflow area, next free slot).");
    out_line("- Not suited to processing every record in order.");
    out_line(ANS "Exam tip: Pick the method to suit the use: real-time lookups \x1e"" direct; batch \x1e"" sequential.");
}
static void n39(void) {
    out_line("- Checks data is sensible/allowed BEFORE accepting it. Can't check it's correct.");
    out_line("- Presence: field not empty.");
    out_line("- Range: within limits (age 11-18).");
    out_line("- Type: right data type (number).");
    out_line("- Length: correct number of characters.");
    out_line("- Format/picture: pattern (e.g. postcode, LLNN NLL).");
    out_line("- Lookup/list: value from a set (Y/N).");
    out_line("- Check digit: extra digit calculated from the others (ISBN, barcodes).");
    out_line("- Cross-field: fields consistent (end date after start date).");
}
static void n40(void) {
    out_line("- Checks data was entered/copied ACCURATELY (matches the source).");
    out_line("- Double entry: type twice, compare (passwords, emails).");
    out_line("- Visual check / proofreading against the original.");
    out_line("- Parity checks and checksums (for transmission).");
    out_line(ANS "Exam tip: Validation = reasonable; verification = accurate copy. Common 2-mark question.");
}
static void n41(void) {
    out_line("- Security: passwords, access rights (read/write), encryption, physical security, backups.");
    out_line("- Backup: copy kept to restore data if lost. Full vs incremental, off-site/cloud, regular schedule.");
    out_line("- Archiving: moving data no longer actively used to long-term storage (kept for legal/historical reasons), frees space on the main system.");
    out_line(ANS "Exam tip: Backup = copy for recovery; archive = old data moved off the live system.");
}
static const calc_t c_0_5[] = {{"Files, records & fields", n34}, {"Master & transaction files", n35}, {"Serial & sequential files", n36}, {"Indexed sequential", n37}, {"Direct (random) access", n38}, {"Validation", n39}, {"Verification", n40}, {"File security & archiving", n41}};
static void n42(void) {
    out_line("- Flat file: one table. Data redundancy (repeats), inconsistency risk.");
    out_line("- Relational: several tables linked by keys. Less redundancy, more consistency, more flexible queries.");
    out_line("- Entity: thing data is stored about (Student). Attribute: property (Name). Table = entity, column = attribute, row = record (tuple).");
}
static void n43(void) {
    out_line("- Primary key: unique identifier for each record in a table.");
    out_line("- Composite key: primary key made of 2+ attributes.");
    out_line("- Foreign key: a primary key from another table, used to link tables.");
    out_line("- Secondary key/index: extra field indexed for fast searching (e.g. surname).");
    out_line("- Index: like a book index; speeds up searches but slows inserts/updates and uses space.");
}
static void n44(void) {
    out_line("- Relationships: one-to-one, one-to-many, many-to-many.");
    out_line("- Draw crow's foot on the \"many\" end.");
    out_line("- Many-to-many must be resolved with a link (junction) table holding both foreign keys, e.g. Student -< Enrolment >- Course.");
    out_line(ANS "Exam tip: Always state the foreign key that implements each relationship.");
}
static void n45(void) {
    out_line("- DBMS: software that manages the database: data dictionary, security, access control, backup, concurrency, integrity.");
    out_line("- User views: different users see only the data/fields they need (e.g. receptionist sees contact details, not medical notes). Improves security and simplicity.");
    out_line("- Data dictionary: metadata (field names, types, sizes, validation, relationships).");
}
static const calc_t c_0_6[] = {{"Database concepts", n42}, {"Keys & indexes", n43}, {"Entity-relationship modelling", n44}, {"DBMS & user views", n45}};
static void n46(void) {
    out_line("- Memory management: allocates RAM to programs, virtual memory.");
    out_line("- Processor management: scheduling, multitasking.");
    out_line("- Backing store/file management: directories, file allocation, access rights.");
    out_line("- Device/peripheral management: drivers, buffers, interrupts.");
    out_line("- Security: user accounts, permissions.");
    out_line("- User interface.");
    out_line("- Error handling, system logs.");
}
static void n47(void) {
    out_line("- GUI (WIMP): windows, icons, menus, pointer. Easy to learn, intuitive; uses more resources.");
    out_line("- Command line: typed commands. Fast for experts, powerful (scripts), low resources; hard to learn.");
    out_line("- Menu driven: choose from options (ATMs). Simple, limited.");
    out_line("- Natural language/voice: speak commands. Natural but accents, noise, ambiguity.");
    out_line("- Touch/gesture: phones, tablets.");
}
static void n48(void) {
    out_line("- Small programs that maintain/improve the system.");
    out_line("- Defragmenter, disk cleanup, antivirus, firewall, backup, compression, encryption, file managers, disk checkers.");
}
static void n49(void) {
    out_line(HEAD "Batch");
    out_line("- Jobs collected and processed together later without user interaction. Efficient for large volumes. e.g. payroll, bills, exam marking.");
    out_line(HEAD "Real-time");
    out_line("- Input processed immediately so output affects the next input. e.g. air traffic control, booking systems (prevent double booking), process control.");
    out_line(HEAD "Interactive/online");
    out_line("- User and computer in dialogue, fast response, e.g. online shopping.");
}
static void n50(void) {
    out_line("- Design the interface around the user: experience level, age, disabilities, environment.");
    out_line("- Consistency (same layout/buttons), clear feedback, error messages that help, minimal memory load, sensible defaults, undo.");
    out_line("- Accessibility: font size, colour contrast, screen reader support, keyboard shortcuts.");
}
static const calc_t c_0_7[] = {{"Functions of the OS", n46}, {"User interfaces", n47}, {"Utility software", n48}, {"Batch vs real-time", n49}, {"Human-computer interaction", n50}};
static void n51(void) {
    out_line("- Variable: named memory location whose value can change.");
    out_line("- Constant: value fixed during execution (e.g. VAT = 0.2). Easier to update, avoids accidental change.");
    out_line("- Identifier: the name. Use meaningful names (totalCost not x).");
    out_line("- Self-documenting code: meaningful names, indentation, comments, white space.");
}
static void n52(void) {
    out_line("- Local variable: declared inside a subroutine; only exists there. Saves memory, avoids side effects, allows same names elsewhere.");
    out_line("- Global variable: declared outside; accessible anywhere. Easy to change accidentally, harder to debug.");
    out_line(ANS "Exam tip: Good practice: use local variables and pass parameters.");
}
static void n53(void) {
    out_line("- Values passed into a subroutine.");
    out_line("- By value: a copy is passed; the original isn't changed.");
    out_line("- By reference: the address is passed; changes affect the original.");
    out_line("- Function returns a value; procedure doesn't.");
}
static void n54(void) {
    out_line("- DIV: integer division (quotient). 17 DIV 5 = 3.");
    out_line("- MOD: remainder. 17 MOD 5 = 2.");
    out_line("- Uses: even/odd (n MOD 2), digits of a number (n MOD 10, n DIV 10), wrapping around (circular queue), converting minutes to hours.");
}
static void n55(void) {
    out_line("- Compare adjacent pairs; swap if in the wrong order. Repeat passes until a pass has no swaps.");
    out_line("- After each pass the largest item \"bubbles\" to the end.");
    out_line("- Simple, but slow: O(n^2). Good for small or nearly sorted lists (with the no-swaps flag).");
    out_line(HEAD "Pseudo-code");
    out_line("- repeat");
    out_line("-   swapped = false");
    out_line("-   for i = 0 to n-2");
    out_line("-     if a[i] > a[i+1] then swap; swapped = true");
    out_line("- until not swapped");
}
static void n56(void) {
    out_line("- Build a sorted part one item at a time: take the next item and insert it into the correct place in the sorted part (shifting larger items right).");
    out_line("- O(n^2) worst case, but fast on nearly sorted data. In place, simple.");
    out_line(HEAD "Pseudo-code");
    out_line("- for i = 1 to n-1");
    out_line("-   item = a[i]; j = i-1");
    out_line("-   while j >= 0 and a[j] > item: a[j+1] = a[j]; j = j-1");
    out_line("-   a[j+1] = item");
}
static void n57(void) {
    out_line("- Check each item in turn from the start until found or the end is reached.");
    out_line("- Works on unsorted data. O(n): slow for large lists.");
}
static void n58(void) {
    out_line("- Data MUST be sorted.");
    out_line("- Look at the middle item; if it's the target, stop; if the target is smaller, discard the upper half, else discard the lower half. Repeat.");
    out_line("- O(log n): very fast. 1000 items needs at most 10 comparisons.");
    out_line(HEAD "Pseudo-code");
    out_line("- low = 0, high = n-1");
    out_line("- while low <= high");
    out_line("-   mid = (low + high) DIV 2");
    out_line("-   if a[mid] = target return mid");
    out_line("-   else if a[mid] < target low = mid + 1 else high = mid - 1");
    out_line("- return not found");
}
static void n59(void) {
    out_line("- Sequence: instructions in order.");
    out_line("- Selection: IF / ELSE / CASE.");
    out_line("- Iteration: count-controlled (FOR), condition-controlled (WHILE checks first, may run 0 times; REPEAT UNTIL checks after, runs at least once).");
    out_line("- Logical operators AND, OR, NOT in conditions.");
}
static void n60(void) {
    out_line("- Split a program into subroutines (procedures/functions) or modules.");
    out_line("- Benefits: easier to test and debug, reuse code (libraries), teams work in parallel, easier maintenance, clearer structure.");
    out_line("- Top-down design / stepwise refinement: break the problem into smaller sub-problems.");
}
static void n61(void) {
    out_line("- Reduces file size: less storage, faster transmission.");
    out_line(HEAD "Lossy");
    out_line("- Permanently removes less important data (JPEG, MP3, MP4). Much smaller. Can't get the original back. Fine for media.");
    out_line(HEAD "Lossless");
    out_line("- Original can be rebuilt exactly (ZIP, PNG). Needed for text/programs.");
    out_line("- Run-length encoding: repeated values stored as value + count (AAAABB \x1e"" 4A2B).");
    out_line("- Dictionary encoding: repeated words/patterns replaced by short codes.");
}
static void n62(void) {
    out_line("- Normal (typical): valid data that should be accepted.");
    out_line("- Boundary (extreme): at the edges of the valid range (and just outside).");
    out_line("- Erroneous (invalid): wrong type or out of range; should be rejected.");
    out_line("- Test plan: test number, purpose, data, expected result, actual result.");
    out_line(ANS "Exam tip: For age 11-18: normal 15, boundary 11 and 18 (and 10, 19), erroneous \"abc\" or -3.");
}
static const calc_t c_0_8[] = {{"Variables, constants & identifiers", n51}, {"Scope", n52}, {"Parameters", n53}, {"DIV & MOD", n54}, {"Bubble sort", n55}, {"Insertion sort", n56}, {"Linear search", n57}, {"Binary search", n58}, {"Programming constructs", n59}, {"Modular programming", n60}, {"Compression", n61}, {"Testing & test data", n62}};
static void n63(void) {
    out_line("- Procedural: sequence of instructions grouped into procedures (Python, C).");
    out_line("- Object-oriented: objects combine data + methods (Java, C#).");
    out_line("- Declarative/logic: state facts and rules, the language finds answers (Prolog, SQL).");
    out_line("- Functional: functions with no side effects (Haskell).");
    out_line("- Event-driven: code responds to events like clicks (GUIs, VB.NET).");
}
static void n64(void) {
    out_line("- Class: blueprint/template defining attributes and methods.");
    out_line("- Object: an instance of a class.");
    out_line("- Attribute: data held by an object. Method: subroutine belonging to the class.");
    out_line("- Encapsulation: data and methods bundled; attributes private, accessed through methods (data hiding).");
    out_line("- Inheritance: subclass takes attributes and methods of a superclass and can add its own (reuse).");
    out_line("- Polymorphism: same method name behaves differently for different classes (overriding).");
}
static void n65(void) {
    out_line(HEAD "High level");
    out_line("- English-like, portable (machine independent), easier to write/debug, one statement = many machine instructions. Needs compiling/interpreting.");
    out_line(HEAD "Low level");
    out_line("- Machine code (binary) and assembly (mnemonics like LDA, ADD).");
    out_line("- Direct hardware control, very efficient/fast, small. Hard to write and processor-specific.");
    out_line("- Used for device drivers, embedded systems.");
}
static const calc_t c_0_9[] = {{"Programming paradigms", n63}, {"Object-oriented concepts", n64}, {"High vs low level languages", n65}};
static void n66(void) {
    out_line("- Is the new system worth doing? TELOS:");
    out_line("- Technical: hardware/software available?");
    out_line("- Economic: costs vs benefits.");
    out_line("- Legal: complies with laws (data protection).");
    out_line("- Operational: will it work with the organisation and staff?");
    out_line("- Schedule: can it be done in time?");
}
static void n67(void) {
    out_line("- Interviews: detailed, follow-up questions; time consuming, few people.");
    out_line("- Questionnaires: many people, anonymous, quick to analyse; low response, can't follow up.");
    out_line("- Observation: see the real process; people may act differently when watched.");
    out_line("- Document analysis: inputs/outputs of the current system; may be out of date.");
}
static void n68(void) {
    out_line("- Data flow diagram: how data moves between external entities, processes and data stores.");
    out_line("- System flowchart: overview of the whole system, inputs, processes, files.");
    out_line("- Program flowchart: logic of an algorithm (start/stop, process, decision, input/output).");
    out_line("- Structure diagram: top-down breakdown into modules.");
    out_line("- ER diagram, HCI/screen designs, data dictionary.");
}
static void n69(void) {
    out_line("- Direct: old system stops, new starts. Cheap, fast; risky if it fails.");
    out_line("- Parallel: both run together. Safe, can compare outputs; costly, double work.");
    out_line("- Phased: introduced in stages. Lower risk; slow.");
    out_line("- Pilot: one branch/department first, then rolled out. Problems found on a small scale; slower.");
    out_line(ANS "Exam tip: Recommend a method for the scenario and justify (e.g. hospital \x1e"" parallel, safety critical).");
}
static void n70(void) {
    out_line("- Unit/module: each part tested alone.");
    out_line("- Integration: parts tested together.");
    out_line("- System: whole system.");
    out_line("- Alpha: by developers/in-house.");
    out_line("- Beta: by real users outside, report bugs.");
    out_line("- Acceptance: client checks it meets requirements before sign-off.");
    out_line("- White box: tests internal logic/paths. Black box: tests inputs vs outputs only.");
}
static void n71(void) {
    out_line("- Corrective: fix bugs found after release.");
    out_line("- Adaptive: change to suit new environment (new OS, laws, hardware).");
    out_line("- Perfective: improve performance/features as users request.");
    out_line("- Good documentation and modular code make maintenance easier.");
}
static void n72(void) {
    out_line("- Regular backups, stored off-site/cloud.");
    out_line("- Full backup: everything; incremental: changes since last backup (faster, slower to restore).");
    out_line("- Grandfather-father-son: keep 3 generations.");
    out_line("- Recovery plan: who does what, how quickly, testing restores.");
}
static void n73(void) {
    out_line(HEAD "User documentation");
    out_line("- How to use the system: installation, tutorials, FAQs, troubleshooting, screenshots. Non-technical.");
    out_line(HEAD "Technical documentation");
    out_line("- For maintenance: algorithms, data dictionary, diagrams, annotated code, test plans, hardware/software requirements.");
}
static const calc_t c_0_10[] = {{"Feasibility study", n66}, {"Investigation methods", n67}, {"Design diagrams", n68}, {"Changeover methods", n69}, {"Types of testing", n70}, {"Maintenance", n71}, {"Backup & recovery", n72}, {"Documentation", n73}};
static void n74(void) {
    out_line("- Editor: syntax highlighting, auto-indent, auto-complete.");
    out_line("- Error diagnostics: highlights syntax errors.");
    out_line("- Translator built in (compile/run).");
    out_line(HEAD "Debugging tools");
    out_line("- Breakpoints: pause at a line.");
    out_line("- Stepping: run one line at a time.");
    out_line("- Watch window: view variable values as the program runs.");
    out_line("- Trace / call stack, variable inspection.");
    out_line(ANS "Exam tip: Explain HOW a tool helps find a logic error, not just name it.");
}
static const calc_t c_0_11[] = {{"IDE features", n74}};
static void n75(void) {
    out_line("- Lexical analysis: remove comments/whitespace, split code into tokens, build symbol table.");
    out_line("- Syntax analysis: check tokens follow the grammar rules; build a parse tree; report syntax errors.");
    out_line("- Semantic analysis: checks meaning, e.g. type errors, undeclared variables.");
    out_line("- Code generation: produce object/machine code.");
    out_line("- Optimisation: make code faster/smaller (remove redundant instructions).");
    out_line("- Linking: combine with library code to produce an executable.");
}
static const calc_t c_0_12[] = {{"Stages of compilation", n75}};
static void n76(void) {
    out_line("- Open source: source code available, can modify/share, usually free, community support. May lack official support.");
    out_line("- Proprietary (closed): code hidden, licence fee, official support and updates, can't modify.");
}
static void n77(void) {
    out_line("- Bespoke (custom): written for one client's exact needs. Expensive, slow to develop, fully tailored, may have more bugs initially.");
    out_line("- Off-the-shelf: ready-made, cheaper, available now, well tested, support/community; may not fit exactly, may include unneeded features.");
}
static void n78(void) {
    out_line("- Industrial: process control, robotics, CAD/CAM.");
    out_line("- Technical/scientific: simulations, modelling, weather forecasting, data analysis.");
    out_line(HEAD "Expert system");
    out_line("- Knowledge base: facts and rules from human experts.");
    out_line("- Inference engine: applies rules to the facts to reach conclusions.");
    out_line("- User interface: questions and answers; explanation facility says how it reached the answer.");
    out_line("- Uses: medical diagnosis, fault finding, financial advice. Pros: consistent, available 24/7. Cons: no common sense, only as good as its rules.");
}
static const calc_t c_0_13[] = {{"Open source vs proprietary", n76}, {"Bespoke vs off-the-shelf", n77}, {"Applications & expert systems", n78}};
static void n79(void) {
    out_line("- Hacking/unauthorised access, identity theft, data loss, theft of devices, interception, insider misuse, phishing.");
    out_line("- Data integrity: data is accurate, complete and consistent.");
}
static void n80(void) {
    out_line("- Passwords and 2FA, user access levels, encryption, firewalls, antivirus, physical security (locks, ID cards), audit logs, staff training, backups, secure disposal.");
}
static void n81(void) {
    out_line("- Threats: fire, flood, hardware failure, power cut, cyber attack.");
    out_line("- Plan: risk analysis, backup strategy (off-site), standby hardware/site (hot/cold sites), UPS, recovery procedures, testing the plan, staff roles.");
}
static void n82(void) {
    out_line("- Malicious: viruses, hacking, deliberate deletion, sabotage by staff.");
    out_line("- Accidental: user error, deleting files, spills, hardware failure, power loss.");
    out_line("- Prevention: access rights, backups, training, validation, confirmation prompts.");
}
static const calc_t c_0_14[] = {{"Risks to personal data", n79}, {"Protection processes", n80}, {"Disaster recovery", n81}, {"Malicious vs accidental damage", n82}};
static void n83(void) {
    out_line("- Data Protection Act 2018 (UK GDPR): data processed lawfully, for specified purposes, accurate, kept no longer than needed, secure; individuals' rights to access/erase data.");
    out_line("- Computer Misuse Act 1990: illegal to gain unauthorised access, access with intent to commit crime, or modify/impair data (e.g. viruses).");
    out_line("- Copyright, Designs and Patents Act 1988: protects software and media from copying/distribution without permission.");
    out_line("- Regulation of Investigatory Powers Act 2000: allows authorities to intercept communications under warrant.");
}
static void n84(void) {
    out_line("- Employment: jobs lost to automation, new jobs in IT; reskilling needed; remote working.");
    out_line("- Digital divide: unequal access by income, age, location.");
    out_line("- Privacy and surveillance, data collection by companies.");
    out_line("- Globalisation, e-commerce, effects on high street shops.");
    out_line("- Environment: e-waste, energy use of data centres; but less travel/paper.");
    out_line(ANS "Exam tip: Give balanced answers: one positive, one negative, and a conclusion.");
}
static const calc_t c_0_15[] = {{"Legislation", n83}, {"Social & economic impacts", n84}};
const topic_t cs_u1[] = {{"Hardware & communication", c_0_0, 12}, {"Logical operations", c_0_1, 3}, {"Data transmission", c_0_2, 7}, {"Data representation & types", c_0_3, 8}, {"Data structures", c_0_4, 3}, {"Organisation of data", c_0_5, 8}, {"Database systems", c_0_6, 4}, {"The operating system", c_0_7, 5}, {"Algorithms & programs", c_0_8, 12}, {"Principles of programming", c_0_9, 3}, {"Systems analysis", c_0_10, 8}, {"Software engineering", c_0_11, 1}, {"Program construction", c_0_12, 1}, {"Types of software", c_0_13, 3}, {"Data security & integrity", c_0_14, 4}, {"Economic, moral, legal & ethical", c_0_15, 2}};
static void n85(void) {
    out_line("- name = input(\"Name: \")      # always a string");
    out_line("- age = int(input(\"Age: \"))    # cast to integer");
    out_line("- price = float(input(\"Price: \"))");
    out_line("- print(\"Total:\", total)");
    out_line("- print(f\"Total: {total:.2f}\")  # 2 decimal places");
    out_line("- str(x), int(x), float(x), bool(x) to convert.");
    out_line(ANS "Exam tip: input() returns a string. Cast before doing maths.");
}
static void n86(void) {
    out_line("- if score >= 70:");
    out_line("-     grade = \"A\"");
    out_line("- elif score >= 60:");
    out_line("-     grade = \"B\"");
    out_line("- else:");
    out_line("-     grade = \"C\"");
    out_line("- Comparison: == != < > <= >=. Logic: and, or, not.");
    out_line("- match value: / case 1: ... (Python 3.10+) for CASE.");
}
static void n87(void) {
    out_line(HEAD "Count controlled");
    out_line("- for i in range(10):        # 0 to 9");
    out_line("- for i in range(1, 11):     # 1 to 10");
    out_line("- for i in range(10, 0, -1): # countdown");
    out_line(HEAD "Condition controlled");
    out_line("- while total < 100:");
    out_line("-     total += 5");
    out_line(HEAD "Repeat-until style");
    out_line("- while True:");
    out_line("-     x = int(input())");
    out_line("-     if x > 0: break");
}
static void n88(void) {
    out_line("- len(s), s.upper(), s.lower(), s.strip()");
    out_line("- s[0] first char, s[-1] last, s[2:5] chars 2-4.");
    out_line("- s.find(\"x\") index (-1 if missing), s.count(\"a\")");
    out_line("- s.replace(\"a\", \"b\"), s.split(\",\"), \",\".join(list)");
    out_line("- ord(\"A\") = 65, chr(65) = \"A\"");
    out_line("- s.isdigit(), s.isalpha()");
    out_line("- for ch in s: loops over characters.");
}
static void n89(void) {
    out_line("- nums = [3, 1, 4]; nums.append(5); nums.remove(1); nums.pop(0)");
    out_line("- len(nums), sum(nums), max(nums), min(nums), sorted(nums)");
    out_line("- if 4 in nums:   # membership");
    out_line("- grid = [[0]*3 for _ in range(3)]   # 3x3");
    out_line("- grid[row][col] = 5");
    out_line("- for row in grid:");
    out_line("-     for item in row: ...");
    out_line(ANS "Exam tip: Don't use [[0]*3]*3: rows become the same list.");
}
static void n90(void) {
    out_line("- def area(w, h):");
    out_line("-     return w * h");
    out_line("- def greet(name):        # procedure: no return");
    out_line("-     print(\"Hi\", name)");
    out_line("- Local variables inside; pass values in as parameters.");
    out_line("- global x  # only if you really must change a global.");
}
static void n91(void) {
    out_line(HEAD "Read");
    out_line("- with open(\"data.txt\", \"r\") as f:");
    out_line("-     for line in f:");
    out_line("-         parts = line.strip().split(\",\")");
    out_line(HEAD "Write (overwrite) / append");
    out_line("- with open(\"out.txt\", \"w\") as f:");
    out_line("-     f.write(name + \",\" + str(score) + \"\\n\")");
    out_line("- \"a\" mode appends instead of overwriting.");
    out_line(ANS "Exam tip: Always strip() the newline, and convert numbers from strings.");
}
static void n92(void) {
    out_line("- while True:");
    out_line("-     s = input(\"Age 11-18: \")");
    out_line("-     if s.isdigit() and 11 <= int(s) <= 18:");
    out_line("-         break");
    out_line("-     print(\"Invalid, try again\")");
    out_line("- Presence: if s.strip() == \"\".");
    out_line("- Length: if len(pw) < 8.");
    out_line("- Format: check each char, or s[0].isalpha() etc.");
    out_line("- try: x = float(s) / except ValueError: for type checks.");
}
static void n93(void) {
    out_line(HEAD "Linear");
    out_line("- def linear(a, t):");
    out_line("-     for i in range(len(a)):");
    out_line("-         if a[i] == t: return i");
    out_line("-     return -1");
    out_line(HEAD "Binary");
    out_line("- def binary(a, t):");
    out_line("-     lo, hi = 0, len(a) - 1");
    out_line("-     while lo <= hi:");
    out_line("-         mid = (lo + hi) // 2");
    out_line("-         if a[mid] == t: return mid");
    out_line("-         elif a[mid] < t: lo = mid + 1");
    out_line("-         else: hi = mid - 1");
    out_line("-     return -1");
}
static void n94(void) {
    out_line(HEAD "Bubble");
    out_line("- for p in range(len(a) - 1):");
    out_line("-     swapped = False");
    out_line("-     for i in range(len(a) - 1 - p):");
    out_line("-         if a[i] > a[i+1]:");
    out_line("-             a[i], a[i+1] = a[i+1], a[i]");
    out_line("-             swapped = True");
    out_line("-     if not swapped: break");
    out_line(HEAD "Insertion");
    out_line("- for i in range(1, len(a)):");
    out_line("-     item = a[i]; j = i - 1");
    out_line("-     while j >= 0 and a[j] > item:");
    out_line("-         a[j+1] = a[j]; j -= 1");
    out_line("-     a[j+1] = item");
}
static void n95(void) {
    out_line("- class Student:");
    out_line("-     def __init__(self, name, mark):");
    out_line("-         self.name = name");
    out_line("-         self.mark = mark");
    out_line("-     def passed(self):");
    out_line("-         return self.mark >= 40");
    out_line("- s = Student(\"Ali\", 72); print(s.name, s.passed())");
    out_line("- Array of records: students = [Student(...), ...]");
    out_line("- Sort records: students.sort(key=lambda s: s.mark)");
}
static void n96(void) {
    out_line("- Random: import random; random.randint(1, 6)");
    out_line("- Rounding: round(x, 2); integer division //, remainder %");
    out_line("- Running total: total = 0; for x in nums: total += x");
    out_line("- Count: count = 0; if cond: count += 1");
    out_line("- Max manual: best = a[0]; for x in a: if x > best: best = x");
    out_line("- Average: sum(a) / len(a)");
    out_line("- Menu loop: while choice != \"Q\": ...");
}
static void n97(void) {
    out_line("- Read the whole task first; plan with comments.");
    out_line("- Build and TEST in small steps; run often.");
    out_line("- Use meaningful names, comments and indentation (marks for quality).");
    out_line("- Validate inputs where asked.");
    out_line("- Test with normal, boundary and erroneous data; keep evidence if required.");
    out_line("- Save regularly. If stuck, get partial functionality working.");
}
static const calc_t c_1_0[] = {{"Input, output & casting", n85}, {"Selection", n86}, {"Loops", n87}, {"String handling", n88}, {"Lists & 2D lists", n89}, {"Functions & procedures", n90}, {"File handling", n91}, {"Validation routines", n92}, {"Searching code", n93}, {"Sorting code", n94}, {"Records & classes", n95}, {"Useful snippets", n96}, {"Exam technique", n97}};
const topic_t cs_u2[] = {{"Python essentials", c_1_0, 13}};
static void n98(void) {
    out_line("- LIFO: last in, first out. One pointer: top.");
    out_line("- Push: if not full, top = top + 1, stack[top] = item.");
    out_line("- Pop: if not empty, item = stack[top], top = top - 1.");
    out_line("- Peek: read top without removing.");
    out_line("- Errors: overflow (push when full), underflow (pop when empty).");
    out_line("- Uses: subroutine calls/return addresses, undo, back button, reversing, evaluating expressions (RPN), recursion.");
}
static void n99(void) {
    out_line("- FIFO: first in, first out. Pointers: front and rear.");
    out_line("- Enqueue at rear, dequeue from front.");
    out_line(HEAD "Linear queue");
    out_line("- Space at the front is wasted as items leave.");
    out_line(HEAD "Circular queue");
    out_line("- Rear wraps round: rear = (rear + 1) MOD size. Reuses space. Track count to tell full from empty.");
    out_line("- Priority queue: items leave by priority, not arrival.");
    out_line("- Uses: print queues, keyboard buffer, CPU scheduling, BFS.");
}
static void n100(void) {
    out_line("- Dynamic: each node holds data + pointer to the next node. Start pointer; last node's pointer = null.");
    out_line("- Free list holds unused nodes (array implementation).");
    out_line(HEAD "Insert");
    out_line("- Get a free node, store data, set its pointer to the next node, update the previous node's pointer to it.");
    out_line(HEAD "Delete");
    out_line("- Change the previous node's pointer to skip the node; return the node to the free list.");
    out_line("- Pros: no shifting items, grows/shrinks. Cons: no direct index access (must traverse), extra memory for pointers.");
}
static void n101(void) {
    out_line("- Each node: left pointer, data, right pointer. Root at the top.");
    out_line("- Binary search tree: left subtree < node < right subtree.");
    out_line("- Insert: compare from the root, go left if smaller, right if bigger, until an empty pointer.");
    out_line("- Search is O(log n) if balanced, O(n) if unbalanced (like a list).");
    out_line(HEAD "Traversals");
    out_line("- Pre-order: node, left, right (copying a tree; prefix).");
    out_line("- In-order: left, node, right (gives sorted order for a BST).");
    out_line("- Post-order: left, right, node (deleting a tree; postfix/RPN).");
    out_line(ANS "Exam tip: Trick: draw a dot left/below/right of each node and trace round the outside.");
}
static void n102(void) {
    out_line("- A hash function turns a key into an index (e.g. key MOD table size).");
    out_line("- Direct access: O(1) on average.");
    out_line("- Collision: two keys hash to the same index.");
    out_line("- Handling: linear probing (next free slot), chaining (linked list at each index), overflow area.");
    out_line("- Good hash function: fast, spreads keys evenly, few collisions. Load factor kept below ~70%.");
}
static void n103(void) {
    out_line("- 3D array: e.g. marks[class][student][test]. Use triple nested loops.");
    out_line("- Static arrays: fixed size, contiguous, O(1) access by index.");
    out_line("- Implementing stacks/queues/lists using arrays + pointer variables.");
}
static const calc_t c_2_0[] = {{"Stacks", n98}, {"Queues", n99}, {"Linked lists", n100}, {"Binary trees", n101}, {"Hash tables", n102}, {"Arrays up to 3D", n103}};
static void n104(void) {
    out_line("- NAND: NOT(A AND B). 0 only when both inputs are 1.");
    out_line("- NOR: NOT(A OR B). 1 only when both inputs are 0.");
    out_line("- Both are universal: any circuit can be built from just NAND (or just NOR) gates.");
    out_line("- NOT A = A NAND A.");
}
static void n105(void) {
    out_line("- NOT(A AND B) = NOT A OR NOT B");
    out_line("- NOT(A OR B) = NOT A AND NOT B");
    out_line("- Rule: break the bar, change the sign (. \x18"" +).");
    out_line("- Use to simplify or to convert to NAND/NOR only.");
    out_line(ANS "Exam tip: Show each law application on its own line.");
}
static void n106(void) {
    out_line("- XOR plaintext with a key; XOR again with the same key gets the original back (A XOR K XOR K = A).");
    out_line("- e.g. 0110 XOR 1010 = 1100; 1100 XOR 1010 = 0110.");
    out_line("- Secure only if the key is random, as long as the message, and used once (one-time pad).");
    out_line("- Masks: AND to clear/test bits, OR to set, XOR to toggle.");
}
static void n107(void) {
    out_line("- Use identities, absorption, De Morgan, and Karnaugh-style grouping by inspection.");
    out_line("- Example: A.B + A.NOT B = A.(B + NOT B) = A.1 = A.");
    out_line("- Example: NOT(NOT A + B) = A . NOT B (De Morgan then double negation).");
    out_line("- Check with a truth table.");
}
static const calc_t c_2_1[] = {{"NAND & NOR", n104}, {"De Morgan's laws", n105}, {"Masking & XOR encryption", n106}, {"Simplifying expressions", n107}};
static void n108(void) {
    out_line("- A subroutine that calls itself.");
    out_line("- Needs a base case (stopping condition) and must move towards it.");
    out_line("- Each call is pushed onto the call stack (parameters, local variables, return address).");
    out_line("- Pros: elegant for naturally recursive problems (trees, factorial, quicksort).");
    out_line("- Cons: more memory, slower, stack overflow if too deep.");
    out_line(HEAD "Example");
    out_line("- def fact(n):");
    out_line("-     if n <= 1: return 1");
    out_line("-     return n * fact(n - 1)");
}
static void n109(void) {
    out_line("- Choose a pivot. Partition: items less than the pivot go left, greater go right. Pivot is now in its final place.");
    out_line("- Recursively quicksort the left and right parts.");
    out_line("- Average O(n log n), worst O(n^2) (bad pivot on sorted data).");
    out_line("- Fast in practice, sorts in place.");
}
static void n110(void) {
    out_line("- Split the list in half repeatedly until lists of 1, then merge pairs back in order.");
    out_line("- Always O(n log n). Needs extra memory for merging. Stable.");
}
static void n111(void) {
    out_line("- Bubble: O(n^2), best O(n) with flag.");
    out_line("- Insertion: O(n^2), best O(n).");
    out_line("- Quicksort: O(n log n) avg, O(n^2) worst.");
    out_line("- Merge: O(n log n).");
    out_line("- Linear search O(n); binary search O(log n) (needs sorted data).");
    out_line("- Hash table lookup O(1) average.");
}
static void n112(void) {
    out_line("- Describes how time (or memory) grows as input size n grows (worst case usually).");
    out_line("- O(1) constant, O(log n) logarithmic, O(n) linear, O(n log n), O(n^2) polynomial/quadratic, O(2^n) exponential.");
    out_line("- Ignore constants and smaller terms: 3n^2 + 5n \x1e"" O(n^2).");
    out_line("- Nested loops over n usually \x1e"" O(n^2).");
    out_line(ANS "Exam tip: Explain in words: \"doubling n quadruples the time\".");
}
static void n113(void) {
    out_line("- Finds shortest paths from a start node in a weighted graph (non-negative weights).");
    out_line("- Set start distance 0, others infinity.");
    out_line("- Repeat: pick the unvisited node with the smallest distance; for each neighbour, if (current + edge) < neighbour's distance, update it and record the previous node. Mark current visited.");
    out_line("- Stop when the target is visited. Trace back via previous nodes for the route.");
    out_line("- Uses: sat nav, network routing.");
}
static void n114(void) {
    out_line("- Depth-first: go as deep as possible before backtracking (uses a stack/recursion).");
    out_line("- Breadth-first: visit all neighbours first, level by level (uses a queue). Finds the shortest path in unweighted graphs.");
    out_line("- Tree traversals: pre-order, in-order, post-order.");
}
static void n115(void) {
    out_line("- RLE: runs of repeated data stored as count + value. Good for simple images, poor for varied data.");
    out_line("- Dictionary (LZW): repeated sequences replaced by codes from a dictionary built as it goes.");
    out_line("- Huffman: frequent characters get shorter binary codes; build a tree from frequencies (combine the two smallest repeatedly).");
    out_line("- Compression ratio = original size / compressed size.");
}
static void n116(void) {
    out_line("- Check digit (e.g. ISBN-13): weight digits 1,3,1,3..., sum, check digit makes the total divisible by 10.");
    out_line("- Modulus 11: weights, sum MOD 11.");
    out_line("- Parity: even/odd number of 1s; detects single-bit errors.");
    out_line("- Checksum: sum of data sent with it, recalculated on arrival.");
    out_line("- Range/format checks written as loops.");
}
static void n117(void) {
    out_line("- Trace table: columns for each variable and output; follow the code line by line.");
    out_line("- Dry run: execute the algorithm on paper to find logic errors.");
    out_line("- Test plan with normal, boundary, erroneous data; expected vs actual.");
    out_line("- Stub testing: test modules with dummy subroutines.");
}
static const calc_t c_2_2[] = {{"Recursion", n108}, {"Quicksort", n109}, {"Merge sort", n110}, {"Sort & search efficiency", n111}, {"Big O notation", n112}, {"Shortest path (Dijkstra)", n113}, {"Graph & tree traversal", n114}, {"Compression algorithms", n115}, {"Validation & verification algorithms", n116}, {"Testing & dry runs", n117}};
static void n118(void) {
    out_line("- Procedural: steps + procedures, shared data.");
    out_line("- Object-oriented: classes/objects, encapsulation, inheritance, polymorphism. Good for large team projects and reuse.");
    out_line("- Declarative: say WHAT you want, not how (SQL, Prolog). Facts + rules + queries.");
    out_line("- Functional: functions as values, no side effects, recursion over loops.");
    out_line("- Event-driven: event handlers/listeners, main loop waiting for events.");
}
static void n119(void) {
    out_line("- Class, object (instance), constructor.");
    out_line("- Encapsulation: private attributes, public getters/setters.");
    out_line("- Inheritance: \"is a\" relationship (Car is a Vehicle).");
    out_line("- Aggregation/composition: \"has a\" relationship.");
    out_line("- Polymorphism: overriding (subclass redefines a method) and overloading.");
    out_line("- Abstract class/interface: defines methods subclasses must implement.");
}
static void n120(void) {
    out_line("- Standards (ANSI, ISO) define a language's syntax and meaning.");
    out_line("- Benefits: code portable across compilers/platforms, programmers can move between systems, consistent behaviour, easier maintenance.");
}
static void n121(void) {
    out_line("- Syntax: rules for valid statements. Ambiguity must be avoided so each statement has one meaning.");
    out_line(HEAD "Backus-Naur form");
    out_line("- <digit> ::= 0|1|2|3|4|5|6|7|8|9");
    out_line("- <integer> ::= <digit> | <digit><integer>   (recursive)");
    out_line("- ::= \"is defined as\", | \"or\", <> non-terminal.");
    out_line(HEAD "Syntax diagrams");
    out_line("- Same rules drawn as railway tracks: rectangles = non-terminals, ovals = terminals, loops = repetition.");
    out_line(ANS "Exam tip: Practice checking whether a given string is valid, step by step.");
}
static const calc_t c_2_3[] = {{"Programming paradigms", n118}, {"OOP in depth", n119}, {"Language standardisation", n120}, {"Syntax, BNF & syntax diagrams", n121}};
static void n122(void) {
    out_line("- Linear stages: requirements, analysis, design, implementation, testing, maintenance.");
    out_line("- Each stage completed and signed off before the next.");
    out_line("- Pros: clear structure, good documentation, easy to manage, suits fixed well-understood requirements.");
    out_line("- Cons: inflexible, user sees the product late, changes are costly.");
}
static void n123(void) {
    out_line("- Iterative: short sprints, working versions delivered frequently.");
    out_line("- Close user involvement and feedback; requirements can change.");
    out_line("- Examples: Scrum, XP (pair programming, test driven).");
    out_line("- Pros: flexible, quick delivery, user satisfaction.");
    out_line("- Cons: less documentation, harder to plan cost/time, needs committed users.");
}
static void n124(void) {
    out_line("- Analysis: requirements specification, investigation findings.");
    out_line("- Design: system/data design, HCI designs, test plan.");
    out_line("- Implementation: annotated code, technical docs.");
    out_line("- Testing: test results, evidence.");
    out_line("- Evaluation/maintenance: user guide, evaluation against requirements.");
}
static const calc_t c_2_4[] = {{"Waterfall model", n122}, {"Agile methods", n123}, {"Documentation at each stage", n124}};
static void n125(void) {
    out_line("- Consider users' skills, age, disabilities, environment, tasks.");
    out_line("- Principles: consistency, feedback, error prevention and clear messages, minimal memory load, shortcuts for experts, simple layout, appropriate colours and fonts.");
    out_line("- Prototyping: build mock-ups to get user feedback early.");
}
static void n126(void) {
    out_line("- Natural language: users type/speak normally; computer interprets meaning.");
    out_line("- Speech recognition: needs training, problems with accents, background noise, homophones, ambiguity.");
    out_line("- Uses: virtual assistants, hands-free (driving, disabilities), call centres.");
    out_line("- Speech synthesis outputs spoken words (screen readers).");
}
static void n127(void) {
    out_line("- Check the design against requirements before building (walkthroughs, prototypes, user review).");
    out_line("- Evaluation criteria: does it meet requirements? Usability, performance/speed, reliability, robustness, cost, maintainability.");
    out_line("- Evaluate with user feedback and test results; suggest improvements.");
}
static const calc_t c_2_5[] = {{"HCI design", n125}, {"Natural language & speech interfaces", n126}, {"Validating the design & evaluation", n127}};
static void n128(void) {
    out_line("- Analysis: CASE tools, diagram software.");
    out_line("- Design: DFD/ER/UML tools, prototyping tools.");
    out_line("- Implementation: IDEs, code libraries, editors.");
    out_line("- Testing: automated testing tools, debuggers, test harnesses.");
    out_line("- Maintenance: version control, bug trackers.");
}
static void n129(void) {
    out_line("- Version control (e.g. Git): tracks every change, who made it and when.");
    out_line("- Can roll back to earlier versions, branch for features, merge changes.");
    out_line("- Lets teams work on the same code without overwriting each other.");
    out_line("- Repository (central or distributed).");
}
static const calc_t c_2_6[] = {{"Development tools", n128}, {"Version management", n129}};
static void n130(void) {
    out_line(HEAD "Compiler");
    out_line("- Translates the whole program into machine code before running; produces an executable.");
    out_line("- Runs fast, code hidden, but errors listed only after compiling; recompile after changes; platform specific.");
    out_line(HEAD "Interpreter");
    out_line("- Translates and runs one line at a time; no executable.");
    out_line("- Easy debugging (stops at the error line), portable; slower, needs the interpreter, source visible.");
    out_line(HEAD "Assembler");
    out_line("- Converts assembly language mnemonics into machine code (one to one).");
    out_line("- Bytecode/virtual machines: compile to intermediate code run on a VM (Java).");
}
static void n131(void) {
    out_line("- Syntax error: breaks the language rules (missing bracket). Found by the translator.");
    out_line("- Logic error: runs but gives the wrong result (< instead of <=). Found by testing/dry runs.");
    out_line("- Run-time (execution) error: crash during running (divide by zero, index out of range, file missing).");
    out_line("- Linking errors, overflow errors.");
}
static const calc_t c_2_7[] = {{"Compiler vs interpreter vs assembler", n130}, {"Types of error", n131}};
static void n132(void) {
    out_line("- e.g. BCS Code of Conduct: act in the public interest, with professional competence and integrity, duty to employer/clients, duty to the profession.");
    out_line("- Includes: respect privacy, avoid conflicts of interest, only take on work you're competent to do, report unsafe practices, keep skills up to date.");
    out_line("- Ethical issues: AI bias, surveillance, automation and jobs, environmental impact, accessibility.");
}
static const calc_t c_2_8[] = {{"Professional codes of conduct", n132}};
const topic_t cs_u3[] = {{"Data structures", c_2_0, 6}, {"Logical operations", c_2_1, 4}, {"Algorithms & programs", c_2_2, 10}, {"Principles of programming", c_2_3, 4}, {"Systems analysis", c_2_4, 3}, {"System design", c_2_5, 3}, {"Software engineering", c_2_6, 2}, {"Program construction", c_2_7, 2}, {"Moral, legal & ethical", c_2_8, 1}};
static void n133(void) {
    out_line(HEAD "CISC");
    out_line("- Many complex instructions, some take several cycles. Shorter programs, more work in hardware. e.g. x86.");
    out_line(HEAD "RISC");
    out_line("- Small set of simple instructions, mostly one cycle each, more registers, easier pipelining, lower power. e.g. ARM in phones.");
    out_line("- Harvard vs Von Neumann: separate vs shared memory for instructions and data.");
}
static void n134(void) {
    out_line("- Fetch, decode and execute of different instructions overlap: while one executes, the next decodes and the next is fetched.");
    out_line("- Increases throughput (instructions per second).");
    out_line("- Problems: branches/jumps mean fetched instructions must be flushed; data dependencies cause stalls.");
}
static void n135(void) {
    out_line(HEAD "Amdahl's law");
    out_line("- Speed-up = 1 / ((1 - p) + p / n)");
    out_line("- p = fraction of the program that can run in parallel, n = number of processors.");
    out_line("- e.g. p = 0.8, n = 4: 1 / (0.2 + 0.2) = 2.5x faster.");
    out_line("- Max speed-up (n \x1e"" infinity) = 1 / (1 - p): the sequential part limits everything.");
    out_line(HEAD "Runtime");
    out_line("- New time = old time x ((1 - p) + p / n).");
    out_line(ANS "Exam tip: Show the formula with numbers substituted.");
}
static void n136(void) {
    out_line("- Mnemonics for machine code: LDA (load), STA (store), ADD, SUB, INP, OUT, BRA (branch always), BRZ (branch if zero), BRP (branch if positive), HLT.");
    out_line(HEAD "Addressing modes");
    out_line("- Immediate: operand is the actual value (#5).");
    out_line("- Direct: operand is the memory address of the data.");
    out_line("- Indirect: operand holds the address of the address.");
    out_line("- Indexed: address = operand + index register (arrays).");
    out_line("- Relative: address relative to the PC.");
    out_line(ANS "Exam tip: Trace programs with a table: ACC, PC, memory locations.");
}
static void n137(void) {
    out_line("- Microphone captures sound, ADC converts to digital, software matches phonemes/words using trained models.");
    out_line("- Uses: dictation, assistants, accessibility, hands-free control.");
    out_line("- Problems: accents, background noise, similar sounding words, needs training, privacy concerns.");
}
static void n138(void) {
    out_line("- WiFi: wireless LAN using radio (2.4 / 5 / 6 GHz). WAPs; range ~50 m; WPA2/WPA3 encryption.");
    out_line("- Bluetooth: short range (~10 m), low power, pairing; peripherals, headphones.");
    out_line("- Mobile 4G/5G: cellular networks; 5G faster, lower latency.");
    out_line("- Issues: interference, range, security (eavesdropping), bandwidth shared between users.");
    out_line("- Benefits for portable devices: mobility, no cables, easy to add users.");
}
static const calc_t c_3_0[] = {{"Architectures: RISC vs CISC", n133}, {"Pipelining", n134}, {"Limits to parallelisation", n135}, {"Assembly language", n136}, {"Voice input systems", n137}, {"Wireless networking", n138}};
static void n139(void) {
    out_line("- Bit rate = bits per second (bps). 1 Mbps = 1,000,000 bps.");
    out_line("- Time = file size (bits) / rate (bps).");
    out_line("- e.g. 20 MB at 8 Mbps: 20 x 8 = 160 Mb; 160 / 8 = 20 s.");
    out_line("- Bandwidth: maximum capacity of the channel. Latency: delay.");
    out_line(ANS "Exam tip: Convert bytes to bits (x8) first. Check if units are 1000 or 1024.");
}
static void n140(void) {
    out_line("- Network shown as a weighted graph; weights = cost (distance, time, money, hops).");
    out_line("- Use Dijkstra's algorithm to find the lowest-cost route between routers.");
    out_line("- Routing tables updated as link costs change (congestion, failures).");
}
static void n141(void) {
    out_line("- Backbone: high-capacity fibre links between major nodes/IXPs; undersea cables.");
    out_line("- ISPs in tiers connect to each other.");
    out_line("- Packet switching and TCP/IP make it robust (no single point of failure).");
    out_line("- Supports WWW, email, VoIP, streaming, cloud, IoT.");
    out_line("- Governance: ICANN (domain names), standards bodies (IETF, W3C).");
}
static const calc_t c_3_1[] = {{"Data transfer rates", n139}, {"Lowest-cost routing", n140}, {"The internet as infrastructure", n141}};
static void n142(void) {
    out_line("- Two's complement addition and subtraction in fixed bits.");
    out_line("- Overflow (signed): adding two positives gives a negative or two negatives gives a positive (sign bit wrong).");
    out_line("- Detected by the overflow flag in the status register.");
}
static void n143(void) {
    out_line("- Normalise: shift mantissa until it starts 01 (positive) or 10 (negative); adjust the exponent by the number of places shifted (left shift = decrease exponent).");
    out_line("- Gives the most precision possible and a unique representation.");
    out_line("- Adding floating point numbers: make exponents equal, add mantissas, normalise.");
}
static void n144(void) {
    out_line("- Logical shift: bits move left/right, zeros fill the gaps. Left = x2, right = /2 for unsigned.");
    out_line("- Arithmetic shift right: sign bit copied in, so negatives stay negative (/2 for two's complement).");
    out_line("- Circular shift (rotate): bits leaving one end re-enter at the other.");
    out_line("- Bits shifted out may set the carry flag; may lose precision.");
}
static void n145(void) {
    out_line("- Overflow: a result is too large for the number of bits available.");
    out_line("- Underflow: a floating point result is too close to zero to be represented (smaller than the smallest normalised value), so it becomes 0.");
    out_line("- Both cause errors; programs should check flags or ranges.");
}
static const calc_t c_3_2[] = {{"Binary arithmetic (advanced)", n142}, {"Floating point normalisation", n143}, {"Shift functions", n144}, {"Overflow & underflow", n145}};
static void n146(void) {
    out_line("- Calculate a storage address from a key: e.g. address = key MOD number_of_buckets.");
    out_line("- Folding: split key into parts and add them.");
    out_line("- Mid-square: square the key and take middle digits.");
    out_line("- Good: quick, even spread, few collisions.");
    out_line("- Collisions \x1e"" overflow area, next free bucket, or chaining.");
}
static void n147(void) {
    out_line("- Large files: a single index would be too big to search quickly.");
    out_line("- Top-level index points to lower-level indexes which point to blocks of records (like cylinder \x1e"" track \x1e"" record).");
    out_line("- Fewer comparisons to find a record; must be updated when records added.");
}
static void n148(void) {
    out_line("- When a block is full, new records go to an overflow area (linked from the block).");
    out_line("- Many overflow records \x1e"" access becomes slow.");
    out_line("- Periodic reorganisation: rewrite the file with all records in order in the main area and rebuild the indexes.");
    out_line("- Deleted records may only be flagged until reorganisation.");
}
static const calc_t c_3_3[] = {{"Hashing algorithms", n146}, {"Multi-level indexes", n147}, {"Overflow & reorganisation", n148}};
static void n149(void) {
    out_line("- Data redundancy: same data stored more than once (wastes space, causes inconsistency).");
    out_line("- Data inconsistency: copies disagree after an update.");
    out_line("- Data integrity: data accurate and consistent (enforced by validation, referential integrity).");
    out_line("- Data independence: structure of the data can change without changing the programs that use it (handled by the DBMS).");
}
static void n150(void) {
    out_line(HEAD "1NF");
    out_line("- No repeating groups; every field atomic (one value); each record unique (primary key).");
    out_line(HEAD "2NF");
    out_line("- In 1NF and no partial dependencies: every non-key field depends on the WHOLE primary key (matters for composite keys).");
    out_line(HEAD "3NF");
    out_line("- In 2NF and no transitive dependencies: non-key fields depend only on the key, not on other non-key fields.");
    out_line("- \"The key, the whole key, and nothing but the key.\"");
    out_line("- Benefits: less redundancy, no update anomalies, easier maintenance.");
}
static void n151(void) {
    out_line("- SELECT name, mark FROM Student WHERE mark > 70 ORDER BY mark DESC;");
    out_line("- SELECT * FROM Student, Class WHERE Student.classID = Class.classID;  (join)");
    out_line("- INNER JOIN Class ON Student.classID = Class.classID");
    out_line("- INSERT INTO Student (id, name) VALUES (5, 'Ali');");
    out_line("- UPDATE Student SET mark = 80 WHERE id = 5;");
    out_line("- DELETE FROM Student WHERE id = 5;");
    out_line("- Functions: COUNT(), SUM(), AVG(), MAX(), MIN(); GROUP BY; LIKE 'A%' wildcards.");
    out_line("- CREATE TABLE Student (id INT PRIMARY KEY, name VARCHAR(30));");
}
static void n152(void) {
    out_line("- Data dictionary, query language, security (user access rights, views), backup and recovery, concurrency control (record locking), integrity checks, indexing.");
    out_line(HEAD "Concurrency");
    out_line("- Record locking: one user edits a record at a time, others wait (prevents lost updates).");
    out_line("- Deadlock: two users each wait for the other's lock.");
    out_line("- Transactions are atomic (ACID): all or nothing.");
}
static void n153(void) {
    out_line("- Big data: huge volume, high velocity, wide variety; too big for traditional databases.");
    out_line("- Data warehouse: large store of historical data from many sources, cleaned and organised for analysis (not day-to-day operations).");
    out_line("- Data mining: searching large data sets for patterns and relationships (e.g. buying habits). Uses: marketing, fraud detection, medical research.");
    out_line("- Issues: privacy, consent, security, accuracy.");
}
static void n154(void) {
    out_line("- Distributed database: data stored across several sites/computers but appears as one.");
    out_line("- Distributed processing: tasks split across many computers on a network.");
    out_line("- Pros: local access is faster, resilient (no single point of failure), scalable.");
    out_line("- Cons: complex to keep data consistent, security across sites, communication delays.");
}
static const calc_t c_3_4[] = {{"Consistency, redundancy & independence", n149}, {"Normalisation (1NF to 3NF)", n150}, {"SQL", n151}, {"DBMS features", n152}, {"Big data, warehousing & mining", n153}, {"Distributed systems", n154}};
static void n155(void) {
    out_line("- Single-user single-task (simple embedded).");
    out_line("- Single-user multitasking (desktop).");
    out_line("- Multi-user (servers, mainframes): many users share resources.");
    out_line("- Real-time: guaranteed response times (control systems).");
    out_line("- Embedded: in devices, limited function, efficient.");
    out_line("- Distributed: one OS across several machines.");
    out_line("- Mobile: power management, touch, apps.");
}
static void n156(void) {
    out_line("- Signal to the CPU that something needs attention (hardware: key press, printer out of paper; software: error; timer).");
    out_line("- Checked at the end of each fetch-execute cycle.");
    out_line("- If priority is higher than the current task: save registers on the stack, run the interrupt service routine (ISR), then restore registers and continue.");
    out_line("- Priorities decide which interrupt is handled first; lower priority ones wait.");
}
static void n157(void) {
    out_line("- Partitioning: memory divided into fixed or variable size sections for processes.");
    out_line("- Paging: memory split into equal-size pages (physical frames); a process's pages can be anywhere; page table maps them.");
    out_line("- Segmentation: variable-size logical blocks (e.g. code, data).");
    out_line("- Virtual memory: pages swapped to disk when RAM is full.");
    out_line("- Prevents processes overwriting each other.");
}
static void n158(void) {
    out_line("- Buffer: temporary memory area holding data being transferred between devices of different speeds (e.g. CPU and printer).");
    out_line("- Double buffering: one buffer filled while the other is emptied.");
    out_line("- Spooling: jobs for a slow device stored on disk in a queue (print queue), so the CPU can continue.");
}
static void n159(void) {
    out_line("- Decides which process gets the CPU next.");
    out_line("- Aims: maximise throughput, fairness, fast response, avoid starvation.");
    out_line("- First come first served: simple, long jobs block others.");
    out_line("- Round robin: each process gets a time slice in turn.");
    out_line("- Shortest job first: minimises average wait; long jobs may starve.");
    out_line("- Shortest remaining time: pre-emptive version.");
    out_line("- Multi-level feedback queues: several queues with priorities.");
}
static void n160(void) {
    out_line("- New \x1e"" Ready \x1e"" Running \x1e"" (Blocked/waiting for I/O) \x1e"" Ready ... \x1e"" Terminated.");
    out_line("- Running \x1e"" Ready when the time slice ends (pre-empted).");
    out_line("- Running \x1e"" Blocked when waiting for I/O; Blocked \x1e"" Ready when it completes.");
    out_line("- Process control block stores state, registers, priority.");
}
static void n161(void) {
    out_line("- Time-slicing: each process gets a short slice of CPU time in turn; gives the impression of simultaneous running.");
    out_line("- Polling: CPU repeatedly checks devices to see if they need attention (wastes time compared with interrupts).");
    out_line("- Threading: a process split into threads that share its memory and can run concurrently (e.g. on different cores).");
}
static const calc_t c_3_5[] = {{"Types of OS", n155}, {"Interrupts", n156}, {"Memory management", n157}, {"Buffering & spooling", n158}, {"Scheduling", n159}, {"Process states", n160}, {"Time-slicing, polling & threading", n161}};
static void n162(void) {
    out_line("- Failure could cause injury, death or major damage: aircraft, medical devices, nuclear plants, car braking.");
    out_line("- Need: rigorous testing, formal methods, redundancy (backup systems), fail-safe design, strict standards.");
}
static void n163(void) {
    out_line("- Sensors measure the environment, ADC converts, processor compares with set values, actuators act (motors, heaters, valves).");
    out_line("- Feedback loop: output affects the next input.");
    out_line("- Examples: greenhouse, traffic lights, central heating, robots.");
}
static void n164(void) {
    out_line("- Knowledge base (facts + rules), inference engine (forward/backward chaining), user interface, explanation facility.");
    out_line("- Built with knowledge engineers and experts.");
    out_line("- Pros: consistent, available, preserves expertise. Cons: no common sense, can't learn (unless ML), hard to maintain rules, liability if wrong.");
}
static void n165(void) {
    out_line("- Machines/software do tasks previously done by people: manufacturing robots, self-checkouts, automated warehouses.");
    out_line("- Pros: faster, consistent, works 24/7, safer for dangerous tasks, cheaper long term.");
    out_line("- Cons: job losses, high setup cost, loss of skills, failures can stop production.");
}
static void n166(void) {
    out_line("- Internet: public global network.");
    out_line("- Intranet: private network inside an organisation using internet technologies (staff only).");
    out_line("- Extranet: part of an intranet opened to selected outsiders (suppliers, customers).");
    out_line("- Wide area information exchange: email, video conferencing, EDI, cloud services.");
}
static const calc_t c_3_6[] = {{"Safety-related systems", n162}, {"Control systems", n163}, {"Expert systems", n164}, {"Automation", n165}, {"Internet, intranet & extranet", n166}};
static void n167(void) {
    out_line("- Several users updating the same data at once can cause lost updates or inconsistency.");
    out_line("- Solutions: record locking, transactions, timestamps, serialisation, commit/rollback.");
    out_line("- Need for immediate backup/logging (transaction logs) so data can be recovered.");
}
static void n168(void) {
    out_line("- Plaintext encrypted into ciphertext using an algorithm and key.");
    out_line(HEAD "Symmetric");
    out_line("- Same key encrypts and decrypts (fast). Problem: sharing the key securely.");
    out_line(HEAD "Asymmetric (public key)");
    out_line("- Public key encrypts, matching private key decrypts. No need to share a secret.");
    out_line("- Used to exchange symmetric keys (e.g. HTTPS/TLS).");
    out_line(HEAD "Digital signatures");
    out_line("- Hash of the message encrypted with the sender's PRIVATE key; receiver decrypts with the PUBLIC key and compares hashes. Proves sender and that data is unchanged.");
    out_line("- Digital certificates (from a certificate authority) prove a public key belongs to an organisation.");
}
static void n169(void) {
    out_line("- Use physical/behavioural features: fingerprint, face, iris, voice, typing pattern.");
    out_line("- Pros: can't be forgotten or easily shared, quick.");
    out_line("- Cons: expensive, false positives/negatives, privacy, can't be changed if stolen, injury affects it.");
}
static void n170(void) {
    out_line("- Virus: attaches to files, spreads when they run.");
    out_line("- Worm: self-replicates across networks without user action.");
    out_line("- Trojan: disguised as useful software.");
    out_line("- Ransomware: encrypts files, demands payment.");
    out_line("- Spyware/keylogger: records activity, steals data.");
    out_line("- Protection: antivirus, firewalls, updates/patches, user training, backups, not opening unknown attachments.");
}
static void n171(void) {
    out_line("- Hacking: unauthorised access to systems (illegal under the Computer Misuse Act).");
    out_line("- Methods: brute force, phishing, SQL injection, exploiting unpatched software, social engineering.");
    out_line("- Penetration testing: authorised simulated attack to find weaknesses before criminals do.");
    out_line("- White box (tester has system info) vs black box (no info, like an outsider).");
    out_line("- Followed by a report and fixes.");
}
static const calc_t c_3_7[] = {{"Online updating issues", n167}, {"Cryptography", n168}, {"Biometrics", n169}, {"Malicious software", n170}, {"Hacking & penetration testing", n171}};
const topic_t cs_u4[] = {{"Hardware & communication", c_3_0, 6}, {"Data transmission", c_3_1, 3}, {"Data representation", c_3_2, 4}, {"Organisation of data", c_3_3, 3}, {"Databases & distributed systems", c_3_4, 6}, {"The operating system", c_3_5, 7}, {"Types of software & systems", c_3_6, 5}, {"Data security & integrity", c_3_7, 5}};
void bool_calc(void);
static const calc_t bool_c[] = {{"Boolean algebra calculator", bool_calc}};
static const topic_t bool_t[] = {{"Boolean algebra calculator", bool_c, 1}};
void app_main(void) {
    extern int ui_notes_mode; ui_notes_mode = 1;
    static unit_t units[5];
    units[0] = (unit_t){"Unit 1: Fundamentals of CS", cs_u1, 16};
    units[1] = (unit_t){"Unit 2: Practical programming", cs_u2, 1};
    units[2] = (unit_t){"Unit 3: Programming & system dev", cs_u3, 9};
    units[3] = (unit_t){"Unit 4: Architecture, data & comms", cs_u4, 8};
    units[4] = (unit_t){"Boolean algebra calculator", bool_t, 1};
    app_run("Computer Science Notes", units, 5);
}
