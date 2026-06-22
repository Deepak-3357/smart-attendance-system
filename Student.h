#pragma once

#include <string>
#include <vector>

struct Student
{
    std::string regNo;
    std::string name;
};

class StudentManager
{
public:
    explicit StudentManager(const std::string& csvPath = "students.csv");

    bool registerStudent(const std::string& regNo, const std::string& name);
    bool updateStudent(const std::string& originalRegNo, const Student& student);
    bool deleteStudent(const std::string& regNo);
    bool exists(const std::string& regNo) const;
    bool getStudentByRegNo(const std::string& regNo, Student& student) const;
    std::vector<Student> search(const std::string& query) const;
    std::vector<Student> getAllStudents() const;
    void viewStudents() const;

    static bool isValidRegNo(const std::string& regNo);
    static std::string trim(const std::string& value);

private:
    std::string csvPath_;

    void ensureFile() const;
    std::vector<Student> loadStudents() const;
    void saveStudents(const std::vector<Student>& students) const;
};
