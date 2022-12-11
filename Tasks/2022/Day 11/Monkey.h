#ifndef MONKEY_H
#define MONKEY_H

#include <deque>
#include <string>

class Monkey
{
public:
    Monkey() = default;
    ~Monkey() = default;
    Monkey(const std::string& description);
public:
    size_t getInspectionCount() const { return m_inspectionCount; }
    size_t getThrowingTarget() const;
    bool inspectItem();
    void throwItem(Monkey& catcher);
private:
    size_t calcNewWorryValue(size_t oldValue);
    void processLine(const std::string& line);
private:
    std::deque<size_t> m_inventory;
    size_t m_inspectionCount = 0;

    // behaviour-"mask"
    // Indicates how the monkey "behaves"
    /*
        Mask:
        - - - - A B C C

         - A    --> Indicates, wether to use the old worry value (1) or the intOperand1 (0) as the first operand
         - B    --> Indicates, wether to use the old worry value (1) or the intOperand2 (0) as the second operand
         - CC   --> Indicates the operation to perform
           - 00 --> Addition
           - 01 --> Subtraction
           - 10 --> Multiplication
           - 11 --> Division
        
        So the mask for squaring the old worry value would be as follows:
            --> 0b00001110
    */
    uint8_t m_behaviourMask = 0b00000000;

    size_t m_intOperand1 = 0;
    size_t m_intOperand2 = 0;

    size_t m_testDivisor = 1;

    size_t m_targetTrue = 0;
    size_t m_targetFalse = 0;

    size_t m_id = 0;
};

#endif