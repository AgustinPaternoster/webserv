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
    int pos = 0;
    while(i < text.size())
    {
        std::size_t start = text.find(serch, i); 
        if (std::string::npos == start)
        {
            std::cout << " no servers";
            break;
        }
        pos = start+6;
        for(;pos < text.size(); pos++)
        {
            if(text[pos] == '{')
                level++;
            if(text[pos] == '}')
            {
                level--;
                if (level == 0)
                {
                    int len = pos - (start + 6);
                    std::cout << text.substr(start + 6 , len ) << std::endl;
                    break;
                }
            }    
        }
        i = pos;
    }
    return (0);
    
    
}