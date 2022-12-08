#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

// Directory-Stuff

class Directory;

class DirectoryEntry
{
public:
    DirectoryEntry(const std::string& name, Directory* parent, bool isDir)
        : m_name(name), m_parent(parent), m_isDir(isDir)
    {}
    virtual ~DirectoryEntry() {}
public:
    const std::string& getName() const { return m_name; }
    Directory*& getParent() { return m_parent; }
    const Directory* getParent() const { return m_parent; }
public:
    virtual const size_t getSize() const = 0;
    const bool isDirectory() const { return m_isDir; };
    std::string getPath() const;
protected:
    std::string m_name;
    Directory* m_parent;
    const bool m_isDir;
};

class File : public DirectoryEntry
{
public:
    File(const std::string& name, Directory* parent, size_t size = 0);
    File(const File& o)
        : DirectoryEntry(o.m_name, o.m_parent, false), m_size(o.m_size)
    {}
    virtual ~File() {}
    virtual const size_t getSize() const override { return m_size; }
private:
    size_t m_size;
};

class Directory : public DirectoryEntry
{
public:
    Directory(const std::string& name, Directory* parent = nullptr)
        : DirectoryEntry(name, parent, true)
    {
        if (m_parent)
            m_parent->addEntry(*this);
    }
    Directory(const Directory& o)
        : DirectoryEntry(o.m_name, o.m_parent, true), m_entries(o.m_entries)
    {}
    virtual ~Directory()
    {
        for (const auto* const& e : m_entries)
            delete e;
    }
public:
    virtual const size_t getSize() const override
    {
        size_t size = 0;
        for (const DirectoryEntry* const& entryP : m_entries)
        {
            const DirectoryEntry& entry = *entryP;

            size_t entrySize;
            if (entry.isDirectory())
                entrySize = ((const Directory&)entry).getSize();
            else
                entrySize = ((const File&)entry).getSize();
            
            size += entrySize;
        }
        return size;
    }
public:
    void addEntry(DirectoryEntry& entry)
    {
        if (&entry == this)
            return;
        for (const auto* const& entryP : m_entries)
        {
            if (entryP->getName() == entry.getName())
            {
                //  std::cout << "Skipping adding entry \"" << newEntry.getName() << "\" to \"" << m_name.c_str() << "\" since it's already contained!\n\t(Matched with \"" << entryP->getName().c_str() << "\")" << std::endl;
                return; // already contained...
            }
        }
        //  std::cout << "Adding new entry to \"" << m_name.c_str() << "\":\n\tName: \"" << entry.getName().c_str() << "\" | Type: " << (entry.isDirectory() ? "Directory" : "File") << std::endl;
        
        entry.getParent() = this;
        DirectoryEntry* newEntry;
        if (entry.isDirectory())
            newEntry = new Directory((const Directory&)entry);
        else
            newEntry = new File((const File&)entry);
        m_entries.push_back(newEntry);

        //  std::cout << "Directory now contains the following entries:\n";
        //  for (const auto* const& entryP : m_entries)
        //  {
        //      std::cout << "\t\"" << entryP->getName() << "\" - Type: " << (entryP->isDirectory() ? "Directory" : "File") << std::endl;
        //  }
    }
    bool isRoot() const { return (m_parent == nullptr); }
    std::string treeView(const std::string& indent = std::string()) const
    {
        std::stringstream ss;
        ss << m_name;
        if (isDirectory())
            ss << "/ (" << getSize() << ')';
        ss << std::endl;
        
        size_t eCount = m_entries.size();
        for (size_t i = 0 ; i < eCount ; i++)
        {
            const DirectoryEntry& entry = *(m_entries[i]);
            if (entry.isDirectory())
            {
                std::string copy = indent;
                if ((i + 1ULL) == eCount)
                {
                    ss << indent << "#-";
                    copy += "  ";
                }
                else
                {
                    ss << indent << "+-";
                    copy += "| ";
                }

                const Directory& dir = (const Directory&)entry;
                ss << dir.treeView(copy);
            }
            else
            {
                if ((i + 1ULL) == eCount)
                    ss << indent << "#-";
                else
                    ss << indent << "+-";

                const File& file = (const File&)entry;
                ss << file.getName() << " (" << file.getSize() << ")" << std::endl;
            }
        }
        return ss.str();
    }
    std::vector<DirectoryEntry*>& getEntries() { return m_entries; }
    const std::vector<DirectoryEntry*>& getEntries() const { return m_entries; }
private:
    std::vector<DirectoryEntry*> m_entries;
};

std::string DirectoryEntry::getPath() const
{
    std::stringstream ss;
    if (getParent())
    {
        ss << getParent()->getPath();
    }
    ss << m_name;
    if (isDirectory())
    {
        ss << '/';
    }
    return ss.str();
}

File::File(const std::string& name, Directory* parent, size_t size)
    : DirectoryEntry(name, parent, false), m_size(size)
{
    if (m_parent)
        m_parent->addEntry(*this);
}

// ~ Directory-Stuff ~

// Splits an entire cmdline line in tokens, seperated by whitespaces
void tokenize(const std::string& cmdLine, std::vector<std::string>& parts)
{
    size_t pos = -1;
    size_t nextPos = 0;
    size_t lineLen = std::strlen(cmdLine.c_str());
    std::string part;
    parts.clear();

    if (lineLen == 0)
        return;

    while ((nextPos = cmdLine.find_first_of(' ', (pos + 1ULL))) != std::string::npos)
    {
        size_t partLen = nextPos - (pos + 1ULL);
        if (partLen != 0) // found a whitespace literally in the next char... so skip it
        {
            part = cmdLine.substr((pos+1), partLen);
            parts.push_back(part);
        }
        pos = nextPos;
    }

    // If pos is equal to the length of the commandline - 1, then the last char must've been a whitespace, so skip it.
    // Otherwise it wasn't, so add the rest of the string from pos until the end of the string to the vector!
    if ((pos + 1) < lineLen)
    {
        part = cmdLine.substr((pos+1), lineLen - (pos+1));
        parts.push_back(part);
    }
}

constexpr static size_t sc_lineLen = 50;

static Directory root("");
static Directory* currentDir = nullptr;

static void cd(const std::string& newDir)
{
    //  std::cout << "Changing directory from \"" << (currentDir ? currentDir->getPath().c_str() : "?") << "\" to ";
    if (newDir == "/")
    {
        //  std::cout << "\"/\"!" << std::endl;
        currentDir = &root;
    }
    else if (newDir == "..")
    {
        if (!currentDir || !currentDir->getParent())
        {
            //  std::cout << "\"/\"!" << std::endl;
            currentDir = &root;
        }
        else
        {
            Directory* dir = currentDir->getParent();
            //  std::cout << '"' << dir->getPath().c_str() << "\"!" << std::endl;
            currentDir = dir;
        }
    }
    else if (newDir != "")
    {
        auto& entries = currentDir->getEntries();
        for (DirectoryEntry*& entryP : entries)
        {
            DirectoryEntry& entry = *entryP;
            if (entry.getName() == newDir && entry.isDirectory())
            {
                Directory* dir = (Directory*)entryP;
                //  std::cout << '"' << dir->getPath().c_str() << "\"!" << std::endl;
                currentDir = dir;
                return;
            }
        }
        //  std::cout << "... nothing new" << std::endl;
    }
    else
    {
        //  std::cout << "... nothing new" << std::endl;
    }
}

size_t calcSumOfDirs(const Directory& root, const size_t maxSize)
{
    static size_t totalSum = 0;
    size_t sum = 0;
    for (const auto* const& entryP : root.getEntries())
    {
        if (entryP->isDirectory())
            calcSumOfDirs((const Directory&)*entryP, maxSize);
        sum += entryP->getSize();
    }
    if (sum <= maxSize)
        totalSum += sum;
    return totalSum;
}

static size_t findSmallestDirGreaterThan(const Directory& root, size_t minSize)
{
    static size_t totalSum = -1;
    size_t sum = root.getSize();
    if (sum >= minSize && sum < totalSum)
        totalSum = sum;
    for (const auto* const& entryP : root.getEntries())
    {
        if (entryP->isDirectory())
            findSmallestDirGreaterThan((const Directory&)*entryP, minSize);
    }
    return totalSum;
}

int main(int argc, const char** argv)
{
    std::ifstream file("input.txt");
    root.getEntries().clear();

    while (file)
    {
        std::string line(sc_lineLen, '\0');
        file.getline(line.data(), sc_lineLen, '\n');

        std::vector<std::string> args;

        //  std::cout << " [Original-Line] - \"" << line.c_str() << "\" - [Original-Line]" << std::endl;

        tokenize(line, args);
        if (args.size() == 0)
            continue;

        const std::string& id = args[0];
        if (id == "$") // This is a command
        {
            const std::string& command = args[1];
            if (command == "cd")
            {
                const std::string& dir = args[2];
                //  std::cout << " [DEBUG] - Command: Change directory to \"" << dir.c_str() << "\" - [DEBUG]" << std::endl;
                cd(dir);
            }
            //  else if (command == "ls")
            //      std::cout << "(ls)" << std::endl;
            //  else
            //      std::cerr << "Unknown command: \"" << command.c_str() << "\"!";
        }
        else if (id == "dir") // Otherwise, this is a directory
        {
            const std::string& dir = args[1];
            //  std::cout << " [DEBUG] - Directory: \"" << dir.c_str() << "\"! - [DEBUG]" << std::endl;
            Directory newDir(dir, currentDir);
            //  currentDir->addEntry(newDir);
        }
        else if (id != "") // Otherwise (if it's not empty), it's a file
        {
            const std::string& file = args[1];
            size_t fSize = atoll(id.c_str());
            //  std::cout << " [DEBUG] - File: \"" << file.c_str() << "\" (" << fSize << " Bytes)! - [DEBUG]" << std::endl;
            File newFile(file, currentDir, fSize);
            //  currentDir->addEntry(newFile);
        }
        else
        {
            std::cout << "Unkown id \"" << id.c_str() << "\"!" << std::endl;
        }
    }

    std::cout << "Done! Complete root-Tree:\n\n" << root.treeView() << std::endl;

    std::cout << "Calculating sum of all directories of at most 100.000 Bytes...\n";
    size_t sum = calcSumOfDirs(root, 100000);
    std::cout << "Done! The sum of the sizes of all directories with a size of at most 100.000 Bytes is " << sum << "!\n";

    size_t toFree = (root.getSize() - (70000000ULL - 30000000ULL));
    std::cout << "Finding the directory with the smallest size, big enough to free " << toFree << " Bytes...\n";
    size_t smallestSize = findSmallestDirGreaterThan(root, toFree);
    std::cout << "Done! The smallest directory big enough for freeing " << toFree << " Bytes is " << smallestSize << " Bytes in size!" << std::endl;

    return 0;
}