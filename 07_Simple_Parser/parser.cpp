/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Mon 22 May 2017 10:48:15 AM CST
 Description: 
 ************************************************************************/

#include <map>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void parse(string file, map<string, map<string, string> > &ans)
{
    string line, section("foobar"), key, value;

    string::size_type pos_equal, pos_last_no_space, pos_quote_start, pos_quote_end, pos_brace_start, pos_brace_end;

    ifstream input(file.c_str());

    while(input)
    {
        getline(input, line, '\n');

        /* ignore comment */
        if (!(line[line.find_first_not_of(' ')] == '#'))
        {
            /* key value pair line*/
            if ((pos_equal = line.find('=')) != string::npos)
            {
                /* get "key" */
                key = line.substr(0, pos_equal);
                pos_last_no_space =  key.find_last_not_of(' ');
                if (pos_last_no_space != string::npos)
                    key = key.substr(0, pos_last_no_space+1);

                /* get "value" */
                pos_quote_start = line.find_first_of('"');
                pos_quote_end = line.find_last_of('"');
                value = line.substr(pos_quote_start+1, pos_quote_end-pos_quote_start-1);

                /* store into map */
                ans[section].insert(make_pair(key, value));

            }
            else
            {
                /* section line*/
                if ((pos_brace_start = line.find('[')) != string::npos)
                {
                    pos_brace_end = line.find_first_of(']');
                    section = line.substr(pos_brace_start+1, pos_brace_end-pos_brace_start-1);
                }
            }
        }
    }
}

int main()
{
    map<string, map<string, string> > ans;
    parse("config.ini", ans);
    cout << ans["FB0"]["xres"] << endl;
}
