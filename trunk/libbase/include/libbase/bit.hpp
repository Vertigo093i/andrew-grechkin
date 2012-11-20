﻿#ifndef _LIBBASE_BIT_HPP_
#define _LIBBASE_BIT_HPP_

#include <libbase/std.hpp>

namespace Base {

	/// Проверка и установка битовых флагов
	namespace WinFlag {
		template<typename Type1, typename Type2>
		bool Check(Type1 in, Type2 flag) {
			return static_cast<Type1>(flag) == (in & static_cast<Type1>(flag));
		}

		template<typename Type1, typename Type2>
		bool CheckAny(Type1 in, Type2 flag) {
			return in & static_cast<Type1>(flag);
		}

		template<typename Type1, typename Type2>
		Type1 & Set(Type1 & in, Type2 flag) {
			return in |= static_cast<Type1>(flag);
		}

		template<typename Type1, typename Type2>
		Type1 & UnSet(Type1 & in, Type2 flag) {
			return in &= ~static_cast<Type1>(flag);
		}

		template<typename Type1, typename Type2>
		Type1 & Switch(Type1 & in, Type2 flag, bool sw) {
			if (sw)
				return Set(in, flag);
			else
				return UnSet(in, flag);
		}
	}

	///================================================================================================
	/// Проверка и установка битов
	namespace WinBit {
		template<typename Type>
		size_t BIT_LIMIT() {
			return sizeof(Type) * 8;
		}

		template<typename Type>
		bool BadBit(size_t in) {
			return !(in < BIT_LIMIT<Type>());
		}

		template<typename Type>
		size_t Limit(size_t in) {
			return (in == 0) ? BIT_LIMIT<Type>() : std::min<int>(in, BIT_LIMIT<Type>());
		}

		template<typename Type>
		bool Check(Type in, size_t bit) {
			if (BadBit<Type>(bit))
				return false;
			Type tmp = 1;
			tmp <<= bit;
			return (in & tmp);
		}

		template<typename Type>
		Type & Set(Type & in, size_t bit) {
			if (BadBit<Type>(bit))
				return in;
			Type tmp = 1;
			tmp <<= bit;
			return (in |= tmp);
		}

		template<typename Type>
		Type & UnSet(Type & in, size_t bit) {
			if (BadBit<Type>(bit))
				return in;
			Type tmp = 1;
			tmp <<= bit;
			return (in &= ~tmp);
		}

		template<typename Type>
		Type & Switch(Type & in, size_t bit, bool sw) {
			if (sw)
				return Set(in, bit);
			else
				return UnSet(in, bit);
		}
	}

}

#endif