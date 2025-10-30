#include <iostream>
#include <fstream>
std::string getDirective(std::string& src, size_t &pos, size_t start)
{
    int level = 0;
    
    for (; pos < src.size(); pos++)
    {
        if(src[pos] == '{')
            level++;
        if(src[pos] == '}')
        {
            level--;
            if(level == 0)
            {
                int len = pos - (start + 6);
                return(src.substr(start + 6, len));
            }
        }
    }
    return ("");
}

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
 //   int i = 0;
    size_t pos = 0;
    while(pos < text.size())
    {
        std::size_t start = text.find(serch, pos); 
        if (std::string::npos == start)
        {
            std::cout << " no servers";
            break;
        }
        pos = start+6;
        std::cout << getDirective(text, pos, start) << std::endl;
        // for(;pos < text.size(); pos++)
        // {
        //     if(text[pos] == '{')
        //         level++;
        //     if(text[pos] == '}')
        //     {
        //         level--;
        //         if (level == 0)
        //         {
        //             int len = pos - (start + 6);
        //             std::cout << text.substr(start + 6 , len ) << std::endl;
        //             break;
        //         }
        //     }    
        // }
    }
    return (0);
}
 
