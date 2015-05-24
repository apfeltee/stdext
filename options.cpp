
#include <core/core.h>
#include <core/commandline.h>

#include <iostream>

int main(int argc, char* argv[])
{
    Core::Commandline prs(argc, argv);
    prs.on("--all", [&](std::string, bool)
    {
        std::cout << "supported flags:\n";
        for(auto flag: prs.getAllFlags())
        {
            std::cout << "   " << '"' << flag << '"' << std::endl << std::flush; 
        }
    });
    prs.onEach({"-o?", "--output="}, [](std::string val, bool)
    {
        std::cout << "value for -o: \"" << val << "\"" << std::endl;
    });

    prs.on("-prefix=", [](std::string val, bool)
    {
        std::cout << "value for -prefix: \"" << val << "\"" << std::endl;
    });

    prs.on("-X?", [](const std::string& val, bool)
    {
        std::cout << "setting JVM flag \"" << val << "\"" << std::endl;
    });

    prs.onEach({"--help", "-h"}, [](std::string, bool)
    {
        std::cout << "help goes here" << std::endl;
    });

    prs.onEach({"-I?", "--include="}, [](std::string val, bool)
    {
        std::cout << "adding include path \"" << val << "\"" << std::endl;
    });

    prs.onEach({"/out:?"}, [](std::string val, bool)
    {
        std::cout << "value for windows-style flag '/out': \"" << val << "\"" << std::endl;
    });

    prs.stopWhen([](Core::Commandline& self)
    {
        //return (self.restSize() != 0);
        return false;
    });
    try
    {
        prs.parse();
        std::cout << "rest arguments:" << std::endl;
        for(auto val: prs.rest())
        {
            std::cout << " + " << val << std::endl;
        }
    }
    catch(std::runtime_error& e)
    {
        std::cout << "parsing failed: " << e.what() << std::endl;
    }
}

