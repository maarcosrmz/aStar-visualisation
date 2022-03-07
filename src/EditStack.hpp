#ifndef EDIT_STACK_HPP
#define EDIT_STACK_HPP

#include "Util.hpp"

enum EditOperations {
    MOVE_START,
    MOVE_TARGET,
    INSERT_OBST,
    DELETE_OBST,
};

class EditStack {
    
    private:
        // Stacks
        std::stack<std::vector<std::pair<i32, i32>>> undo_data;
        std::stack<std::vector<std::pair<i32, i32>>> redo_data;
        std::stack<short> undo_op;
        std::stack<short> redo_op;

    public:
        EditStack() {}
        ~EditStack() {}

        void WriteStack(const std::vector<std::pair<i32, i32>> data, short op);
        std::vector<std::pair<i32, i32>> Undo(short* op);
        std::vector<std::pair<i32, i32>> Redo(short* op);

        inline size_t getUndoSize() const { return undo_op.size(); }
        inline size_t getRedoSize() const { return redo_op.size(); }

};

#endif //EDIT_STACK_HPP
