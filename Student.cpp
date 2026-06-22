#include "Student.h"
#include "CsvUtils.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

StudentManager::StudentManager(const std::string& csvPath)
    : csvPath_(csvPath)
{
    ensureFile();
}

bool StudentManager::registerStudent(const std::string& regNoInput, const std::string& nameInput)
{
    const std::string regNo = trim(regNoInput);
    const std::string name = trim(nameInput);

    if (!isValidRegNo(regNo))
    {
        std::cout << "Invalid register number. Use digits only and keep it within 32-bit numeric range.\n";
        return false;
    }

    if (name.empty())
    {
        std::cout << "Student name cannot be empty.\n";
        return false;
    }

    std::vector<Student> students = loadStudents();
    const auto duplicate = std::any_of(students.begin(), students.end(), [&](const Student& student) {
        return student.regNo == regNo;
    });

    if (duplicate)
    {
        std::cout << "Register number already exists.\n";
        return false;
    }

    students.push_back({ regNo, name });
    saveStudents(students);
    std::cout << "Student registered successfully.\n";
    return true;
}

bool StudentManager::updateStudent(const std::string& originalRegNo, const Student& updated)
{
    const std::string regNo = trim(updated.regNo);
    const std::string name = trim(updated.name);
    if (!isValidRegNo(regNo) || name.empty()) return false;

    auto students = loadStudents();
    auto current = std::find_if(students.begin(), students.end(), [&](const Student& s) { return s.regNo == originalRegNo; });
    if (current == students.end()) return false;
    const bool duplicate = std::any_of(students.begin(), students.end(), [&](const Student& s) {
        return s.regNo == regNo && s.regNo != originalRegNo;
    });
    if (duplicate) return false;
    *current = {regNo, name};
    saveStudents(students);
    return true;
}

bool StudentManager::deleteStudent(const std::string& regNo)
{
    auto students = loadStudents();
    const auto end = std::remove_if(students.begin(), students.end(), [&](const Student& s) { return s.regNo == regNo; });
    if (end == students.end()) return false;
    students.erase(end, students.end());
    saveStudents(students);
    return true;
}

bool StudentManager::exists(const std::string& regNo) const
{
    Student student;
    return getStudentByRegNo(regNo, student);
}

bool StudentManager::getStudentByRegNo(const std::string& regNo, Student& student) const
{
    const std::vector<Student> students = loadStudents();
    const auto found = std::find_if(students.begin(), students.end(), [&](const Student& current) {
        return current.regNo == regNo;
    });

    if (found == students.end())
    {
        return false;
    }

    student = *found;
    return true;
}

std::vector<Student> StudentManager::search(const std::string& query) const
{
    const std::string normalizedQuery = trim(query);
    std::vector<Student> result;

    if (normalizedQuery.empty())
    {
        return result;
    }

    std::string lowerQuery = normalizedQuery;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });

    for (const Student& student : loadStudents())
    {
        std::string lowerName = student.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), [](unsigned char ch) {
            return static_cast<char>(std::tolower(ch));
        });

        if (student.regNo.find(normalizedQuery) != std::string::npos ||
            lowerName.find(lowerQuery) != std::string::npos)
        {
            result.push_back(student);
        }
    }

    return result;
}

std::vector<Student> StudentManager::getAllStudents() const
{
    return loadStudents();
}

void StudentManager::viewStudents() const
{
    const std::vector<Student> students = loadStudents();

    std::cout << "\n=================================\n";
    std::cout << "Registered Students\n";
    std::cout << "=================================\n";

    if (students.empty())
    {
        std::cout << "No students registered.\n";
        return;
    }

    std::cout << std::left << std::setw(16) << "Reg No" << "Name\n";
    std::cout << "---------------------------------\n";

    for (const Student& student : students)
    {
        std::cout << std::left << std::setw(16) << student.regNo << student.name << '\n';
    }
}

bool StudentManager::isValidRegNo(const std::string& regNo)
{
    if (regNo.empty() || regNo.size() > 10)
    {
        return false;
    }

    if (!std::all_of(regNo.begin(), regNo.end(), [](unsigned char ch) { return std::isdigit(ch) != 0; }))
    {
        return false;
    }

    try
    {
        return std::stoll(regNo) <= 2147483647LL;
    }
    catch (...)
    {
        return false;
    }
}

std::string StudentManager::trim(const std::string& value)
{
    return CsvUtils::trim(value);
}

void StudentManager::ensureFile() const
{
    CsvUtils::ensureHeader(csvPath_, "RegNo,Name");
}

std::vector<Student> StudentManager::loadStudents() const
{
    ensureFile();

    std::ifstream input(csvPath_);
    if (!input)
    {
        throw std::runtime_error("Unable to open students file: " + csvPath_);
    }

    std::vector<Student> students;
    std::string line;
    bool firstLine = true;

    while (std::getline(input, line))
    {
        if (firstLine)
        {
            firstLine = false;
            continue;
        }

        if (CsvUtils::trim(line).empty())
        {
            continue;
        }

        const std::vector<std::string> fields = CsvUtils::parseLine(line);
        if (fields.size() >= 2)
        {
            students.push_back({ fields[0], fields[1] });
        }
    }

    return students;
}

void StudentManager::saveStudents(const std::vector<Student>& students) const
{
    std::ofstream output(csvPath_, std::ios::trunc);
    if (!output)
    {
        throw std::runtime_error("Unable to write students file: " + csvPath_);
    }

    output << "RegNo,Name\n";
    for (const Student& student : students)
    {
        output << CsvUtils::escape(student.regNo) << ',' << CsvUtils::escape(student.name) << '\n';
    }
}
