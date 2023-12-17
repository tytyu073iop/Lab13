#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
#include <vector>

struct SMark { //NOLINT: givven
    std::string Subject;
    int Mark;
};

struct SStudData { //NOLINT: in task givven
    std::string Name;
    int Number; //NOLINT: in task givven
    std::vector <SMark> Marks;
};

int main() {
    std::ifstream in("in.txt");
    if (!in.is_open()) { std::cout << "Error opening in.txt\n"; return 1; }
    std::map<int, SStudData> m;
    std::string temp_line;
    while (std::getline(in, temp_line)) {
        std::stringstream ss(temp_line);
        SStudData student;
        std::getline(ss, student.Name, ';');
        ss >> student.Number;
        ss.clear();
        ss.ignore();
        std::vector<SMark> marks;
        std::string temp_mark;
        while (std::getline(ss, temp_mark, ',')) {
            SMark mark;
            std::stringstream ss(temp_mark);
            std::getline(ss, mark.Subject, ':');
            ss >> mark.Mark;
            marks.push_back(mark);
        }
        student.Marks = marks;
        m[student.Number] = student;
    }
    if (m.empty()) { std::cout << "Empty file!\n"; return 2; }
    std::vector<SStudData> vm;
    for (const auto& [num, s] : m) {
        vm.push_back(s); //NOLINT: Lazy
    }
    {
        std::cout << "Sorted by number:\n";
        for (const auto& [num, s] : m) {
            std::cout << s.Name << ';' << s.Number << ';';
            float sum = 0;
            for (const auto& m : s.Marks) {
                std::cout << m.Subject << ':' << m.Mark << ',';
                sum += static_cast<float>(m.Mark);
            }
            std::cout << "Middle:" << sum / static_cast<float>(s.Marks.size());
            std::cout << '\n';
        }
    }
        struct SStudDataM : SStudData {
            float MidMark = 0; //NOLINT: just
            SStudDataM(const SStudData& i) : SStudData(i) { // NOLINT: I wnat convert
                float sum = 0;
                std::for_each(Marks.begin(), Marks.end(), [&sum](const SMark& i){ sum += static_cast<float>(i.Mark); });
                MidMark = sum / static_cast<float>(Marks.size());
            }
            bool operator<(const SStudDataM& rhs) const {
                return Name == rhs.Name ? MidMark > rhs.MidMark : Name < rhs.Name;
            }
            SStudDataM() : SStudData() {}
        };
    {
        std::cout << "Sorted by alphabet\n";
        std::multiset<SStudDataM> const s(vm.begin(), vm.end());
        size_t c = 0;
        for (const auto& i : s) {
            std::cout << c << ';';
            std::stringstream ss(i.Name);
            std::string surname;
            ss >> surname;
            std::cout << surname << ';' << i.Number << ';' << i.MidMark << '\n';
            c++;
        }
    }
    struct ByPoint {
            bool operator()(const SStudDataM& lhs, const SStudDataM& rhs) {
                return lhs.MidMark > rhs.MidMark;
            }
        };
    {
        std::cout << "Enter Mid Point range:";
        float l = 0;
        float r = 0;
        std::cin >> l >> r;
        std::multiset<SStudDataM, ByPoint> s(vm.begin(), vm.end());
        auto rbegin = std::lower_bound(s.rbegin(), s.rend(), l, [](const SStudDataM& i, float j){ return i.MidMark < j; });
        auto rend = std::upper_bound(s.rbegin(), s.rend(), r, [](float j, const SStudDataM& i){ return j < i.MidMark; });
        auto begin = rend.base();
        auto end = rbegin.base();
        std::cout << (begin == end ? "No students in that range\n" : "this range:\n");
        std::for_each(begin, end, [](const SStudDataM& i){ std::cout << i.Name << ';' << i.MidMark << '\n'; });
    }
    {
        std::string sub;
        std::cout << "What subject:";
        std::cin >> sub;
        std::multiset<SStudDataM> s(vm.begin(), vm.end());
        std::vector<SStudDataM> v(vm.begin(), vm.end());
        auto end = std::copy_if(s.begin(), s.end(), v.begin(), [sub](const SStudDataM& i){ auto v = i.Marks; return std::any_of(v.begin(), v.end(), [sub](const SMark& i){ return i.Subject == sub; }); });
        std::cout << (v.begin() == end ? "Nobody passed subject\n" : "Students passed subject:\n");
        std::for_each(v.begin(), end, [](const SStudDataM& i){ std::cout << i.Name << '\n'; });
    }
    {
        std::cout << "Subject, ammount passed\n";
        std::map<std::string, size_t> s;
        for (const auto& [num, stu] : m) {
            for (const auto& i : stu.Marks) {
                s[i.Subject]++;
            }
        }
        for (const auto& [s, c] : s) {
            std::cout << s << ':' << c << '\n';
        }
    }
    {
        std::cout << "Subject, middle\n";
        std::map<std::string, std::pair<int, size_t>> ma;
        for(const auto& [num, student] : m) {
            for(const auto& i : student.Marks) {
                ma[i.Subject].second++;
                ma[i.Subject].first += i.Mark;
            }
        }
        std::multimap<float, std::string, std::greater<>> ms;
        for(const auto& [sub, mark] : ma) {
            ms.insert({static_cast<float>(mark.first) / static_cast<float>(mark.second), sub});
        }
        std::ranges::for_each(ms, [](const auto& i){ std::cout << i.second << ';' << i.first << '\n'; });
    }
    {
        std::cout << "Students with max\n";
        std::multiset<SStudDataM, ByPoint> s(vm.begin(), vm.end());
        auto rend = std::lower_bound(s.rbegin(), s.rend(), s.begin()->MidMark, [](const SStudDataM& j, const float& i){ return j.MidMark < i; });
        std::for_each(s.begin(), rend.base(), [](const SStudDataM& i) { std::cout << i.Name << '\n'; });
    }
    {
        std::cout << "Failed students:\n";
        std::vector v(vm);
        auto end = std::ranges::copy_if(vm, v.begin(), [](const SStudDataM& i){ auto v = i.Marks; return std::ranges::any_of(v, [](const SMark& i){ return i.Mark < 4; }); });
        std::for_each(v.begin(), end.out, [](const SStudDataM& i){ std::cout << i.Name << '\n'; });
    }
    return 0;
}

