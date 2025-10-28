#include <iostream>
#include <fstream>

int main(void)
{
    std::string path ="../test.txt";
    std::string text;
    std::string serch = "server";
    std::ifstream file(path);
    std::istreambuf_iterator<char> init(file);
    std::istreambuf_iterator<char> end;
    text = std::string(init,end);
    std::size_t text_init = text.find(serch);
    std::size_t text_end = text.find(serch,text_init+1);
    std::cout << text_init << std::endl;
    std::cout << text_end << std::endl;
    std::cout << text.substr(text_init, text_end - text_init) << std::endl;
    
    
}