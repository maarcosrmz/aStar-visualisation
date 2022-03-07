#include "EditStack.hpp"

void EditStack::WriteStack(const std::vector<std::pair<i32, i32>> data, short op)
{
    std::stack<std::vector<std::pair<i32, i32>>> rev_data;
    std::stack<short> rev_op;

    while (!redo_data.empty()) {
        undo_data.push(redo_data.top());
        undo_op.push(redo_op.top());

        rev_data.push(redo_data.top());
        rev_op.push(redo_op.top());

        redo_data.pop();
        redo_op.pop();
    }

    while (!rev_data.empty()) {
        auto d = rev_data.top();
        auto o = rev_op.top();

        switch (o) {
            case MOVE_START:
            case MOVE_TARGET:
                {
                    auto helper = d[0];
                    d[0] = d[1];
                    d[1] = helper;
                }
                break;

            case INSERT_OBST:
                o = DELETE_OBST;
                break;

            case DELETE_OBST:
                o = INSERT_OBST;
                break;

        }

        undo_data.push(d);
        undo_op.push(o);

        rev_data.pop();
        rev_op.pop();
    }

    undo_data.push(data);
    undo_op.push(op);
}

std::vector<std::pair<i32, i32>> EditStack::Undo(short* op)
{
    auto data = undo_data.top();    undo_data.pop();
    *op       = undo_op.top();      undo_op.pop();

    redo_data.push(data);
    redo_op.push(*op);

    return data;
}

std::vector<std::pair<i32, i32>> EditStack::Redo(short* op)
{
    auto data = redo_data.top();    redo_data.pop();
    *op       = redo_op.top();      redo_op.pop();

    undo_data.push(data);
    undo_op.push(*op);

    return data;
}


