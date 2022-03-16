#pragma once

#ifndef POLOS_CONTEXT_VERTEXBUFFEROBJECT_H
#define POLOS_CONTEXT_VERTEXBUFFEROBJECT_H

namespace polos
{
	class vertex_buffer_object
	{
	public:
		void bind();
		void unbind();
	private:
		uint32 id;
	};
}

#endif /* POLOS_CONTEXT_VERTEXBUFFEROBJECT_H */
