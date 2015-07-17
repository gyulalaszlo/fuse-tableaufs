#pragma once

#include <memory>

#include <capnp/message.h>
#include <capnp/serialize-packed.h>

#include "cpp14/make_unique.hpp"

namespace kj
{
  class BufferedInputStream;
  class OutputStream;
}

namespace monkeykingz
{
  template <class StateT>
  struct Dispatcher {
    using InputStream = kj::BufferedInputStream;
    using OutputStream = kj::OutputStream;

   public:
    Dispatcher(std::unique_ptr<StateT> state) : ops(), state(std::move(state))
    {
    }

    template <class InputT, class OutputT, class HandlerT>
    void register_op(int op_id, HandlerT handler)
    {
      ops.emplace(std::make_pair(
          op_id,
          std::make_unique<OpAdapter<InputT, OutputT, HandlerT>>(handler)));
    }

    void run(int opcode, InputStream& input, OutputStream& output)
    {
      // if we dont have the op, log it and return
      if (ops.count(opcode) == 0) {
        fprintf(stderr, "Unknown opcode: %d", opcode);
        return;
      }

      // we have the op, run it
      ops[opcode]->run(*state.get(), input, output);
    }

   private:
    // The interface to wrap the dispatchable handlers into a unified interface
    class Op
    {
      MKZ_INTERFACE(Op);
      virtual void run(StateT& state, InputStream& input,
                       OutputStream& output) = 0;
    };

    template <class Input, class Output, class Handler>
    class OpAdapter : public Op
    {
     public:
      OpAdapter(Handler handler) : handler(std::move(handler)) {}
      virtual void run(StateT& state, InputStream& in_stream,
                       OutputStream& output)
      {
        ::capnp::PackedMessageReader reader(in_stream);
        ::capnp::MallocMessageBuilder builder;

        auto in = reader.template getRoot<Input>();
        auto out = builder.template initRoot<Output>();

        handler(state, in, out);
        writePackedMessage(output, builder);
      }
      Handler handler;
    };

    std::map<int, std::unique_ptr<Op>> ops;
    std::unique_ptr<StateT> state;
  };
}
