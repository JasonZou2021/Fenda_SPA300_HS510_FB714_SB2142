#ifndef __TASK_ATTRIBUTE_H__
#define __TASK_ATTRIBUTE_H__

#define STACK_BYTE2DEPTH(byte)				((byte) >> 2)
#define TASK_NAME(module, name, category)	(#module"_"#name"_"#category)

#endif

