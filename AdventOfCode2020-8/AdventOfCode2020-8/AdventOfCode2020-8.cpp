// AdventOfCode2020-8.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <cassert>

struct ProgramState
{
    int accumulator = 0;
    size_t pc = 0;
};

class Instruction
{
public:
    Instruction(int v)
        : value(v)
    {
    }

    void Execute(ProgramState& state) const
    {
        ExecuteInternal(state);
    };

    int GetValue() const { return value; }

protected:
    int value = 0;

private:
    bool executed = false;

private:
    virtual void ExecuteInternal(ProgramState& state) const = 0;
};

class Accumulate : public Instruction
{
public:
    Accumulate(int v) : Instruction(v) {}

private:
    void ExecuteInternal(ProgramState& state) const final
    {
        state.accumulator += value;
    }
};

class TrackedInstruction
{
public:
    TrackedInstruction(std::shared_ptr<Instruction>&& i) : instr(std::move(i)) {};

    bool Execute(ProgramState& state)
    {
        if (executed)
        {
            return false;
        }

        instr->Execute(state);

        executed = true;
        return true;
    }

public:
    std::shared_ptr<Instruction> instr;     //< We use shared ptrs here since we need TrackedInstructions to be copyable

private:
    bool executed = false;
};

class Jump : public Instruction
{
public:
    Jump(int v) : Instruction(v) {}

private:
    void ExecuteInternal(ProgramState& state) const final
    {
        state.pc += value;
    }
};

class NoOp : public Instruction
{
public:
    NoOp(int v) : Instruction(v) {}

private:
    void ExecuteInternal(ProgramState& state) const final
    {
    }
};

// We need to copy the instruction stack here since we are modifying it (marking as executed), which we want to revert in case this executions fails.
bool ExecuteStack(std::vector<TrackedInstruction> instructionStack, ProgramState& state)
{
    while (state.pc < instructionStack.size())
    {
        const size_t prevPC = state.pc;
        const bool success = instructionStack[state.pc].Execute(state);

        if (!success)
        {
            break;
        }

        // Only advance PC if instruction didn't modify it
        if (prevPC == state.pc)
        {
            ++state.pc;
        }
    }

    if (state.pc == instructionStack.size())
    {
        return true;
    }

    return false;
}

int main()
{
    std::vector<TrackedInstruction> instructionStack;

    // Input
    for (std::string input; std::cin >> input;)
    {
        int value;
        std::cin >> value;

        std::shared_ptr<Instruction> instructionPtr;
        if (input == "acc")
        {
            instructionPtr = std::make_shared<Accumulate>(value);
        }
        else if (input == "jmp")
        {
            instructionPtr = std::make_shared<Jump>(value);
        }
        else if (input == "nop")
        {
            instructionPtr = std::make_shared<NoOp>(value);
        }

        assert(instructionPtr != nullptr);
        instructionStack.emplace_back(std::move(instructionPtr));
    }

    // Process
    ProgramState state;
    while (state.pc < instructionStack.size())
    {
        // Playing a bit with dynamic cast to know which instruction types I need to convert. dynamic_cast has fame of being slow, but it is practical in this situations.
        if (dynamic_cast<Jump*>(instructionStack[state.pc].instr.get()))
        {
            // Convert to nop and execute
            std::shared_ptr<Instruction> convertedInstruction = std::make_shared<NoOp>(instructionStack[state.pc].instr->GetValue());
            // Prevent unnecessary copies by swapping (moving) the shared pointers (also avoid messing with the reference counters). This also allow us to easily revert the instruction afterwards.
            std::swap(convertedInstruction, instructionStack[state.pc].instr);
            ProgramState stateCopy = state;

            if (ExecuteStack(instructionStack, stateCopy))
            {
                state = stateCopy;
                break;
            }
            
            // If not correct, restore previous instruction
            std::swap(convertedInstruction, instructionStack[state.pc].instr);
        }
        else if (dynamic_cast<NoOp*>(instructionStack[state.pc].instr.get()))
        {
            // Convert to jump and execute
            std::shared_ptr<Instruction> convertedInstruction = std::make_shared<Jump>(instructionStack[state.pc].instr->GetValue());
            std::swap(convertedInstruction, instructionStack[state.pc].instr);
            ProgramState stateCopy = state;

            if (ExecuteStack(instructionStack, stateCopy))
            {
                state = stateCopy;
                break;
            }

            // If not correct, restore previous instruction
            std::swap(convertedInstruction, instructionStack[state.pc].instr);
        }

        const size_t prevPC = state.pc;
        const bool success = instructionStack[state.pc].Execute(state);

        if (!success)
        {
            break;
        }

        // Only advance PC if instruction didn't modify it (i.e. not a jmp)
        if (prevPC == state.pc)
        {
            ++state.pc;
        }
    }

    std::cout << state.accumulator << "\n";
}