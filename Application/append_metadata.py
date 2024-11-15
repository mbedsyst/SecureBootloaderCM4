import struct
import binascii

APP_START_ADDR = 0x08008000
MAGIC_NUMBER = 0xDEADBEEF
VERSION = 1

# Read the application binary
with open("HelloWorld.bin", "rb") as f:
    app_data = f.read()

# Calculate size of the application code
app_size = len(app_data)

# Calculate CRC32
app_crc = binascii.crc32(app_data) & 0xFFFFFFFF

# Create metadata
metadata = struct.pack("<IIII", MAGIC_NUMBER, VERSION, app_size, app_crc)

# Combine metadata and application binary
with open("application_with_metadata.bin", "wb") as f:
    f.write(metadata + app_data)

print(f"Metadata added: Magic={hex(MAGIC_NUMBER)}, Version={VERSION}, Size={app_size}, CRC={hex(app_crc)}")

