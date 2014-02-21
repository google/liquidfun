/*
* Copyright (c) 2014 Google, Inc.
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/
#ifndef B2_INTRUSIVE_LIST
#define B2_INTRUSIVE_LIST

#include <Box2D/Common/b2Settings.h>

// Use b2IntrusinveListNode to implement a doubly-linked list.
//
// Example usage:
//
// 	class MyClass {
// 	public:
// 		MyClass(const char *msg) : m_msg(msg) {}
// 		const char* GetMessage() const { return m_msg; }
// 		B2_INTRUSIVE_LIST_GET_NODE(MyClass, m_node);
// 		B2_INTRUSIVE_LIST_NODE_GET_CLASS(MyClass, m_node);
// 	private:
// 		b2IntrusiveListNode m_node;
// 		const char *m_msg;
// 	};
//
// 	int main(int argc, char *argv[]) {
// 		b2IntrusiveListNode list; // NOTE: type is NOT MyClass
// 		MyClass a("this");
// 		MyClass b("is");
// 		MyClass c("a");
// 		MyClass d("test");
// 		list.InsertBefore(a.GetListNode());
// 		list.InsertBefore(b.GetListNode());
// 		list.InsertBefore(c.GetListNode());
// 		list.InsertBefore(d.GetListNode());
// 		for (b2IntrusiveListNode* node = list.GetNext();
// 	    	 node != list.GetTerminator(); node = node->GetNext()) {
// 			MyClass *cls = MyClass::GetInstanceFromNode(node);
// 			printf("%s\n", cls->GetMessage());
// 		}
// 		return 0;
// 	}


class b2IntrusiveListNode {
public:
	b2IntrusiveListNode() {
		Initialize();
	}

	~b2IntrusiveListNode() {
		Remove();
	}

	void InsertAfter(b2IntrusiveListNode *node) {
		b2Assert(!node->InList());
		node->m_next = m_next;
		node->m_prev = this;
		m_next->m_prev = node;
		m_next = node;
	}

	void InsertBefore(b2IntrusiveListNode *node) {
		b2Assert(!node->InList());
		node->m_next = this;
		node->m_prev = m_prev;
		m_prev->m_next = node;
		m_prev = node;
	}

	void Remove() {
		m_prev->m_next = m_next;
		m_next->m_prev = m_prev;
		Initialize();
	}

	bool InList() const {
		return m_next != m_prev;
	}

	int32 GetLength() const {
		int32 len = 0;
		for (const b2IntrusiveListNode* n = GetNext();
			 n != GetTerminator(); n = n->GetNext()) {
			len++;
		}
		return len;
	}

	b2IntrusiveListNode* GetNext() { return m_next; }
	b2IntrusiveListNode* GetPrevious() { return m_prev; }
	b2IntrusiveListNode* GetTerminator() { return this; }
	const b2IntrusiveListNode* GetNext() const { return m_next; }
	const b2IntrusiveListNode* GetPrevious() const { return m_prev; }
	const b2IntrusiveListNode* GetTerminator() const { return this; }

private:
	void Initialize() {
		m_next = this;
		m_prev = this;
	}

private:
	b2IntrusiveListNode *m_prev;
	b2IntrusiveListNode *m_next;
};


#define B2_INTRUSIVE_LIST_GET_NODE(Class, NodeMemberName) \
	b2IntrusiveListNode* GetListNode() { return &NodeMemberName; }

#define B2_INTRUSIVE_LIST_NODE_GET_CLASS_ACCESSOR( \
	Class, NodeMemberName, FunctionName) \
	static Class* FunctionName(b2IntrusiveListNode *node) { \
		Class *cls = NULL; \
		/* This effectively performs offsetof(Class, NodeMemberName) */ \
		/* which ends up in the undefined behavior realm of C++ but in */ \
		/* practice this works with most compilers. */ \
		return reinterpret_cast<Class*>( \
			reinterpret_cast<uint8*>(node) - \
			reinterpret_cast<uint8*>(&cls->NodeMemberName)); \
	}

#define B2_INTRUSIVE_LIST_NODE_GET_CLASS(Class, NodeMemberName) \
	B2_INTRUSIVE_LIST_NODE_GET_CLASS_ACCESSOR(Class, NodeMemberName, \
											  GetInstanceFromNode)

#endif
