import struct
import binascii
from elftools.elf.elffile import ELFFile

APP_START_ADDR = 0x08008000
MAGIC_NUMBER = 0xDEADBEEF
VERSION = 1

# Step 1: Convert ELF to BIN
elf_file = "HelloWorld.elf"
bin_file = "HelloWorld.bin"

with open(elf_file, "rb") as elf:
    elffile = ELFFile(elf)
    # Locate the `.text` section which contains the executable code
    with open(bin_file, "wb") as bin_out:
        for segment in elffile.iter_segments():
            if segment['p_type'] == 'PT_LOAD':  # Look for loadable segments
                bin_out.write(segment.data())

print(f"Converted ELF ({elf_file}) to BIN ({bin_file})")

# Step 2: Read the application binary
with open(bin_file, "rb") as f:
    app_data = f.read()

# Step 3: Calculate size of the application code
app_size = len(app_data)

# Step 4: Calculate CRC32
app_crc = binascii.crc32(app_data) & 0xFFFFFFFF

# Step 5: Create metadata
metadata = struct.pack("<IIII", MAGIC_NUMBER, VERSION, app_size, app_crc)

# Step 6: Combine metadata and application binary
output_file = "application_with_metadata.bin"
with open(output_file, "wb") as f:
    f.write(metadata + app_data)

print(f"Metadata added: Magic={hex(MAGIC_NUMBER)}, Version={VERSION}, Size={app_size}, CRC={hex(app_crc)}")
print(f"Output written to {output_file}")

