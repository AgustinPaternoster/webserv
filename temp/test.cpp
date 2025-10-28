#include <iostream>
#include <fstream>

int getserver(std::string file)
{
    std::size_t found = 0;
    int total;
    int inc = 0;
    while(std::string::npos  != (found = file.find("server", found+inc)))
    {
        inc++;
        total++;
    }
    return (total);
}
int main(void)
{
    std::string path ="../test.txt";
    std::string text;
    std::string serch = "server";
    std::ifstream file(path);
    std::istreambuf_iterator<char> init(file);
    std::istreambuf_iterator<char> end;
    text = std::string(init,end);

    int level;
    int i = 0;
    while(i < text.size())
    {
        std::size_t found = text.find(serch); 
        if (std::string::npos == found)
        {
            std::cout << " no servers";
            break;
        }
        for(int i = found; i < text.size(); i++)
        {
            if(text[i] == '{')
                level++;
            if(text[i] == '}')
            {

            }    
        }
    }
    return (0);
    
    
}