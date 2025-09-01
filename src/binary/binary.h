#pragma once 
#include "common.h"

namespace nomp {
    namespace interface {
		PRO_DEF_MEM_DISPATCH(MemWriteUint64, writeUint64);
		PRO_DEF_MEM_DISPATCH(MemWriteUint32, writeUint32);
		PRO_DEF_MEM_DISPATCH(MemWriteUint16, writeUint16);
		PRO_DEF_MEM_DISPATCH(MemReadUint64, readUint64);
		PRO_DEF_MEM_DISPATCH(MemReadUint32, readUint32);
		PRO_DEF_MEM_DISPATCH(MemReadUint16, readUint16);
		PRO_DEF_MEM_DISPATCH(MemUint64, uint64);
		PRO_DEF_MEM_DISPATCH(MemUint32, uint32);
		PRO_DEF_MEM_DISPATCH(MemUint16, uint16);
        struct Encoder : pro::facade_builder
            ::add_facade<Copyable>
            ::add_convention<MemWriteUint64, void(std::span<std::byte> buf, uint64_t v)>
            ::add_convention<MemWriteUint32, void(std::span<std::byte> buf, uint32_t v)>
            ::add_convention<MemWriteUint16, void(std::span<std::byte> buf, uint16_t v)>
			::add_convention<MemWriteUint64, void(IWriter writer, uint64_t v)>
			::add_convention<MemWriteUint32, void(IWriter writer, uint32_t v)>
			::add_convention<MemWriteUint16, void(IWriter writer, uint16_t v)>
			::add_convention<MemUint64, uint64_t(std::span<const std::byte> buf)>
			::add_convention<MemUint32, uint32_t(std::span<const std::byte> buf)>
			::add_convention<MemUint16, uint16_t(std::span<const std::byte> buf)>
			::add_convention < MemReadUint64, void(IReader, uint64_t&)>
			::add_convention < MemReadUint32, void(IReader, uint32_t&)>
			::add_convention < MemReadUint16, void(IReader, uint16_t&)>
            ::build {
		};
    }
    class BigEndian {
        public:
            static void writeUint64(std::span<std::byte> buf, uint64_t v) {
                if (buf.size() < 8) throw std::invalid_argument("Buffer too small for uint64");
                buf[0] = static_cast<std::byte>((v >> 56) & 0xFF);
                buf[1] = static_cast<std::byte>((v >> 48) & 0xFF);
                buf[2] = static_cast<std::byte>((v >> 40) & 0xFF);
                buf[3] = static_cast<std::byte>((v >> 32) & 0xFF);
                buf[4] = static_cast<std::byte>((v >> 24) & 0xFF);
                buf[5] = static_cast<std::byte>((v >> 16) & 0xFF);
                buf[6] = static_cast<std::byte>((v >> 8) & 0xFF);
                buf[7] = static_cast<std::byte>(v & 0xFF);
            }

            static void writeUint64(interface::IWriter writer, uint64_t v) {
                std::byte buf[8];
                writeUint64(std::span{ buf, 8 }, v);
                writer->write(std::span{ buf, 8 });
			}

            static uint64_t uint64(std::span<const std::byte> buf) {
                if (buf.size() < 8) throw std::invalid_argument("Buffer too small for uint64");
                return uint64_t(buf[7]) | uint64_t(buf[6])<<8 | uint64_t(buf[5])<<16 | uint64_t(buf[4])<<24
                     | uint64_t(buf[3])<<32 | uint64_t(buf[2])<<40 | uint64_t(buf[1])<<48 | uint64_t(buf[0])<<56;
            }

            static void readUint64(interface::IReader reader, uint64_t& v) {
                std::byte buf[8];
                if (reader->read(std::span{ buf, 8 }) != 8) {
                    throw std::runtime_error("Failed to read uint64");
                }
                v = uint64(std::span{ buf, 8 });
			}

            static void writeUint32(std::span<std::byte> buf, uint32_t v) {
                if (buf.size() < 4) throw std::invalid_argument("Buffer too small for uint32");
                buf[0] = static_cast<std::byte>((v >> 24) & 0xFF);
                buf[1] = static_cast<std::byte>((v >> 16) & 0xFF);
                buf[2] = static_cast<std::byte>((v >> 8) & 0xFF);
                buf[3] = static_cast<std::byte>(v & 0xFF);
            }

            static void writeUint32(interface::IWriter writer, uint32_t v) {
                std::byte buf[4];
                writeUint32(std::span{ buf, 4 }, v);
                writer->write(std::span{ buf, 4 });
            }

            static uint32_t uint32(std::span<const std::byte> buf) {
                if (buf.size() < 4) throw std::invalid_argument("Buffer too small for uint32");
                return uint32_t(buf[3]) | uint32_t(buf[2])<<8 | uint32_t(buf[1])<<16 | uint32_t(buf[0])<<24;
            }

            static void readUint32(interface::IReader reader, uint32_t& v) {
                std::byte buf[4];
                if (reader->read(std::span{ buf, 4 }) != 4) {
                    throw std::runtime_error("Failed to read uint32");
                }
                v = uint32(std::span{ buf, 4 });
            }

            static void writeUint16(std::span<std::byte> buf, uint16_t v) {
                if (buf.size() < 2) throw std::invalid_argument("Buffer too small for uint16");
                buf[0] = static_cast<std::byte>((v >> 8) & 0xFF);
                buf[1] = static_cast<std::byte>(v & 0xFF);
            }

            static void writeUint16(interface::IWriter writer, uint16_t v) {
                std::byte buf[2];
                writeUint16(std::span{ buf, 2 }, v);
                writer->write(std::span{ buf, 2 });
			}

            static uint16_t uint16(std::span<const std::byte> buf) {
                if (buf.size() < 2) throw std::invalid_argument("Buffer too small for uint16");
                return uint16_t(buf[1]) | uint16_t(buf[0])<<8;
            }

            static void readUint16(interface::IReader reader, uint16_t& v) {
                std::byte buf[2];
                if (reader->read(std::span{ buf, 2 }) != 2) {
                    throw std::runtime_error("Failed to read uint16");
                }
                v = uint16(std::span{ buf, 2 });
            }
    };
}