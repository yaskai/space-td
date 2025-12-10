#ifndef VEC_H_
#define VEC_H_

#include <stddef.h>
#include <stdlib.h>

#define vec_define(_name, _type)																			\
typedef struct {																							\
	_type *data;																							\
	size_t count;																							\
	size_t capacity;																						\
} _name;

#define vec_declare(_name, _type)																			\
																											\
	vec_define(_name, _type)																				\
	_name _v_##_name = (_name) {																			\
		.data = NULL,																						\
		.count = 0,																							\
		.capacity = 0,																						\
	};																										\
																											\
	void _v_##_name##_push (_type thing) {																	\
		if(_v_##_name.count + 1 > _v_##_name.capacity) {													\
			_v_##_name.capacity *= 2;																		\
																											\
			if(_v_##_name.capacity > 0)	{																	\
				_type *ptr = realloc(_v_##_name.data, sizeof(_type) * _v_##_name.capacity);					\
				_v_##_name.data = ptr;																		\
			} else {																						\
				_v_##_name.capacity = 1;																	\
				_v_##_name.data = malloc(sizeof(_type) * _v_##_name.capacity);								\
			}																								\
		}																									\
																											\
		_v_##_name.data[_v_##_name.count++] = thing;														\
	}

#endif
