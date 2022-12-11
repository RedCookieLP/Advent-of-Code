#include <Monkey.h>
#include <iostream>

static size_t s_globMod = 1;

enum BEHAVIOUR_FLAGS
{
    OLD_WORRY_OP1   = 0b00001000,
    OLD_WORRY_OP2   = 0b00000100,

    OP_ADD          = 0b00000000,
    OP_SUB          = 0b00000001,
    OP_MUL          = 0b00000010,
    OP_DIV          = 0b00000011,

    OPERAND         = 0b00000011
};

Monkey::Monkey(const std::string& description)
{
    size_t newLinePos = -1;
    do
    {
        std::string currentLine;
        size_t oldNewLine = newLinePos;
        newLinePos = description.find_first_of('\n',newLinePos+1);
        if (newLinePos == description.npos)
            currentLine = description.substr(oldNewLine + 1);
        else
            currentLine = description.substr(oldNewLine + 1, (newLinePos - oldNewLine) - 1);
        
        processLine(currentLine);
    }
    while (newLinePos != description.npos);

    s_globMod *= m_testDivisor;
}
size_t Monkey::getThrowingTarget() const
{
    const size_t& item = m_inventory.front();
    bool isDivisible = ((item % m_testDivisor) == 0);
    size_t target = (isDivisible ? m_targetTrue : m_targetFalse);
    //  std::cout << "Testing value \"" << item << "\" if it's divisible by " << m_testDivisor << "...\n"
    //            << "\t--> Test " << (isDivisible ? "passed" : "failed") << "! Target is Monkey #" << (isDivisible ? m_targetTrue : m_targetFalse) << '!' << std::endl;
    //  std::cout << "Monkey #" << m_id << " --> Throwing item with value \"" << item << "\" to monkey #" << target << '!' << std::endl;
    return target;
}
bool Monkey::inspectItem()
{
    if (m_inventory.empty())
        return false;

    m_inspectionCount++;
    
    size_t& item = m_inventory.front(); // Get the currently inspected item's worry value
    //  std::cout << "Inspecting item with value " << item << "..." << std::endl;
    item = calcNewWorryValue(item);     // Calculate the new value based on your reaction
    //  item /= 3;                          // Divide by 3 for the relief of it not being broken
    item %= s_globMod;
    //  std::cout << "The new value is " << item << '!' << std::endl;

    return true;
}
void Monkey::throwItem(Monkey& catcher)
{
    size_t item = m_inventory.front();      // Get and...
    m_inventory.pop_front();                // ...throw the current item to the catcher

    catcher.m_inventory.push_back(item);    // Make the catcher catch it
}

size_t Monkey::calcNewWorryValue(size_t oldValue)
{
    size_t op1 = ((m_behaviourMask & OLD_WORRY_OP1) ? oldValue : m_intOperand1);
    size_t op2 = ((m_behaviourMask & OLD_WORRY_OP2) ? oldValue : m_intOperand2);

    switch ((m_behaviourMask & OPERAND))
    {
        case OP_ADD:
            return (op1 + op2);
        break;
        case OP_SUB:
            return (op1 - op2);
        break;
        case OP_MUL:
            return (op1 * op2);
        break;
        case OP_DIV:
            return (op1 / op2);
        break;
    }

    return oldValue;
}

static bool startsWith(const std::string& str1, const char* cmp)
{
    size_t cmpLen = std::strlen(cmp);
    return (str1.substr(0,cmpLen) == cmp);
}

#define STRING_CONST(name, str) constexpr static inline const char * const name = str ;\
static inline const size_t name ## _len = std::strlen( name )

STRING_CONST(sc_monkeyStart,"Monkey ");
STRING_CONST(sc_inventoryStart,"  Starting items:");
STRING_CONST(sc_operationStart,"  Operation: new = ");
STRING_CONST(sc_testStart,"  Test: divisible by ");
STRING_CONST(sc_trueTestStart,"    If true: throw to monkey ");
STRING_CONST(sc_falseTestStart,"    If false: throw to monkey ");

void Monkey::processLine(const std::string& line)
{
    if (line == "")
        return;
    
    std::string subLine;
    if (startsWith(line, sc_monkeyStart))
    {
        subLine = line.substr(sc_monkeyStart_len);
        size_t subLineLen = std::strlen(subLine.c_str());
        m_id = std::atoll(subLine.substr(0, subLineLen-1).c_str());
    }
    else if (startsWith(line, sc_inventoryStart))
    {
        subLine = line.substr(sc_inventoryStart_len);
        size_t commaPos = -1;
        do
        {
            std::string currentValue;
            size_t oldCommaPos = commaPos;
            commaPos = subLine.find_first_of(',',commaPos+1);
            if (commaPos == subLine.npos)
                currentValue = subLine.substr(oldCommaPos + 1);
            else
                currentValue = subLine.substr(oldCommaPos + 1, (commaPos - oldCommaPos) - 1);

            if (currentValue == "")
                break;
            
            m_inventory.push_back(std::atoll(currentValue.c_str() + 1));
        }
        while (commaPos != subLine.npos);
        //  std::cout << " [STARTING_INVENTORY_LINE]" << std::endl;
    }
    else if (startsWith(line, sc_operationStart))
    {
        subLine = line.substr(sc_operationStart_len);

        size_t firstSpacePos = subLine.find_first_of(' ');
        size_t secondSpacePos = subLine.find_first_of(' ', firstSpacePos + 1);

        std::string firstOperand = subLine.substr(0, firstSpacePos);
        std::string operation = subLine.substr(firstSpacePos + 1, (secondSpacePos - firstSpacePos) - 1);
        std::string secondOperand = subLine.substr(secondSpacePos + 1);
        
        m_behaviourMask = 0;
        if (firstOperand == "old")
            m_behaviourMask |= OLD_WORRY_OP1;
        else
            m_intOperand1 = std::atoll(firstOperand.c_str());
        
        if (secondOperand == "old")
            m_behaviourMask |= OLD_WORRY_OP2;
        else
            m_intOperand2 = std::atoll(secondOperand.c_str());

        switch (operation[0])
        {
            case '+':
                m_behaviourMask |= OP_ADD;
            break;
            case '-':
                m_behaviourMask |= OP_SUB;
            break;
            case '*':
                m_behaviourMask |= OP_MUL;
            break;
            case '/':
                m_behaviourMask |= OP_DIV;
            break;
        };

        //  std::cout << " [OPERATION_LINE] --> \"" << firstOperand << "\" \"" << operation << "\" \"" << secondOperand << "\" ==> Resulting behaviour-mask: " << (uint32_t)m_behaviourMask << std::endl;
    }
    else if (startsWith(line, sc_testStart))
    {
        subLine = line.substr(sc_testStart_len);
        m_testDivisor = std::atoll(subLine.c_str());
        //  std::cout << " [TEST_LINE] - Divisible by " << m_testDivisor << std::endl;
    }
    else if (startsWith(line, sc_trueTestStart))
    {
        subLine = line.substr(sc_trueTestStart_len);
        m_targetTrue = std::atoll(subLine.c_str());
        //  std::cout << " [TEST_TRUE_LINE] - The target is monkey #" << m_targetTrue << std::endl;
    }
    else if (startsWith(line, sc_falseTestStart))
    {
        subLine = line.substr(sc_falseTestStart_len);
        m_targetFalse = std::atoll(subLine.c_str());
        //  std::cout << " [TEST_FALSE_LINE] - The target is monkey #" << m_targetFalse << std::endl;
    }
}