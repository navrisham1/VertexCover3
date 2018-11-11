#include <stdio.h>      /* printf */
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>


int getRandInt(int min, int max)
{
    static  std::ifstream urandom("/dev/urandom");

    if( min == max )
    {
        return min;
    }
    else
    {
        unsigned int num;
        urandom.read((char*)&num, sizeof(int));
        return min + num % (max - min + 1);
    }
}


/***************************
 *      Street class
 ***************************/
class Street
{
private:
    int line_count;
    std::vector< std::pair<int,int> > coordinates;
public:
    std::string name;
    Street(std::string name, int line_count, int cord_max);
    void outputStreet();
    bool exists(std::pair<int,int> coordinate);

};


Street::Street(std::string street_name, int line_count, int cord_max)
{
    const int max_attempts = 25;
    int attempts=0;
    this->name = street_name;
    this->line_count = line_count;
    for(int i=0; i<=line_count; )
    {
        int x = getRandInt(-cord_max, cord_max);
        int y = getRandInt(-cord_max, cord_max);
        std::pair<int,int> coordinate (x,y);
        if(this->exists(coordinate))
        {
            attempts++;
            if(attempts >= max_attempts) throw "Too many attempts";
        }
        else
        {
            attempts = 0;
            i++;
            this->coordinates.push_back(coordinate);
        }

    }
}


void Street::outputStreet()
{
    std::cout << "a \"" << this->name << "\" ";
    for(std::pair<int,int> p: this->coordinates)
    {
        std::cout << "(" <<  p.first << "," << p.second << ")";
    }
    std::cout << std::endl << std::flush;
}

bool Street::exists(std::pair<int,int> coordinate)
{
    for(std::pair<int,int> p: this->coordinates)
    {
        if(coordinate == p) return true;
    }
    return false;
}


class A3Rgen
{
private:
    bool debug;
    int streets_max;
    int lines_max;
    int cord_max;
    int delay;
    std::ifstream urandom;
    std::vector<Street> streets;

    void printLine(std::string line);
    void generateStreets();
    void removeStreets();

public:
    const int STREETS_MIN = 2;
    const int STREETS_DEFAULT = 10;

    const int LINES_MIN = 1;
    const int LINES_DEFAULT = 5;

    const int CORD_MIN = 1;
    const int CORD_DEFAULT = 20;

    const int DELAY_MIN = 5;

    A3Rgen();
    void initialize(int s_value, int n_value, int c_value, int l_value, bool debug);
    void run();
};

A3Rgen::A3Rgen()
{
    this->urandom.open("/dev/urandom", std::ifstream::in);
    if (this->urandom.fail())
    {
        throw "cannot open /dev/urandom\n";
    }

    this->debug = debug;
    this->streets_max   = this->STREETS_DEFAULT;
    this->lines_max     = this->LINES_DEFAULT;
    this->cord_max      = this->CORD_DEFAULT;
    this->delay         = this->DELAY_MIN;
}

void A3Rgen::printLine(std::string line)
{
    std::cout << line << std::endl << std::flush;
}

void A3Rgen::generateStreets()
{
    int street_count = getRandInt(this->STREETS_MIN, this->streets_max);

    for( int i=0; i < street_count; i++ )
    {
        std::string street_name;
        street_name += static_cast<char>( (i % 26) + 65 );
        street_name += static_cast<char>( (i / 26) + 97 );
        street_name += " St";

        int line_count = getRandInt(this->LINES_MIN, this->lines_max);
        Street new_street(street_name, line_count, this->cord_max);
        this->streets.push_back(new_street);
    }

    for( Street s: this->streets)
    {
        s.outputStreet();
    }
    std::cout << "g" << std::endl << std::flush;
    return;
}

void A3Rgen::removeStreets()
{
    for( Street s: this->streets)
    {
        std::cout << "r \"" << s.name << "\"" << std::endl << std::flush;
    }
    this->streets.clear();
}

void A3Rgen::run()
{
    while(true)
    {
        this->removeStreets();
        this->generateStreets();
        sleep( getRandInt(this->DELAY_MIN, this->delay) );
    }
}


void A3Rgen::initialize(int s_value, int n_value, int c_value, int l_value, bool debug=false)
{
    this->streets_max = s_value;
    this->lines_max = n_value;
    this->cord_max = c_value;
    this->delay = l_value;
    this->debug = debug;
}

int main (int argc, char **argv)
{
    bool debug = false;
    A3Rgen a3_rgen;
    try
    {
        int s_value = a3_rgen.STREETS_DEFAULT;
        int n_value = a3_rgen.LINES_DEFAULT;
        int l_value = a3_rgen.DELAY_MIN;
        int c_value = a3_rgen.CORD_DEFAULT;
        std::string option_value;
        int index;
        int c;
        opterr = 0;

        std::string exception("");
        while ((c = getopt (argc, argv, "s:n:l:c:v")) != -1)
        {
            switch (c)
            {
            case 's':
                option_value = optarg;
                s_value = atoi(option_value.c_str());
                if(s_value < a3_rgen.STREETS_MIN)
                {
                    exception += "-s option must be at least " + std::to_string(a3_rgen.STREETS_MIN) + "; ";
                }
                break;

            case 'n':
                option_value = optarg;
                n_value = atoi(option_value.c_str());
                if(n_value < a3_rgen.LINES_MIN)
                {
                    exception += "-n option must be at least " + std::to_string(a3_rgen.LINES_MIN) + "; ";
                }
                break;

            case 'l':
                option_value = optarg;
                l_value = atoi(option_value.c_str());
                if(l_value < a3_rgen.DELAY_MIN)
                {
                    exception += "-l option must be at least " + std::to_string(a3_rgen.DELAY_MIN) + "; ";
                }
                break;

            case 'c':
                option_value = optarg;
                c_value = atoi(option_value.c_str());
                if(c_value < a3_rgen.CORD_MIN)
                {
                    exception += "-c option must be at least " + std::to_string(a3_rgen.CORD_MIN) + "; ";
                }
                break;

            case 'v':
                debug = true;
                break;

            case '?':
                if (optopt == 's' || optopt == 'n' || optopt =='l' || optopt =='c')
                {
                    exception += "Missing option value for '-";
                    exception += static_cast<char>(optopt);
                    exception += "'; ";
                }
                else
                {
                    exception += "Unknown option: -";
                    exception += static_cast<char>(optopt);
                    exception += "; ";
                }
                break;

            default:
                exception += "Unknown error in parsing rgen options; ";
                break;
            }
        }
        if (exception != "")
        {
            throw exception;
        }

        if (debug)
        {
            std::cout << "\n\n"
                      << "-s value=" << s_value << ", "
                      << "-n value=" << n_value << ", "
                      << "-l value=" << l_value << ", "
                      << "-c value=" << c_value << std::endl;

            if (optind < argc)
            {
                std::cout << "Found positional arguments\n";
                for (index = optind; index < argc; index++)
                    std::cout << "Non-option argument: " << argv[index] << "\n";
            }
        }

        a3_rgen.initialize(s_value, n_value, c_value, l_value, debug);
        a3_rgen.run();
    }

    catch(std::string error)
    {
        std::cerr << "Error: [rgen] " << error << std::endl;
        exit(EXIT_FAILURE);
    }

    catch(const char* error)
    {
        std::cerr << "Error: [rgen] " << error << std::endl;
        exit(EXIT_FAILURE);
    }
    return 0;
}
