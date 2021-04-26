#include <iostream>
#include <thread>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

using namespace std;





//string DataCollection = "web,wed,green,sun,moon,land,part,web,green";

string DataCollection;


//string DataCollection = myfile;
vector<string> WordList;
map<string, int> ResultTable;

const int input_size = 735;
const int no_of_threads = 3;

class Parser {
public:
	void operator()(char delimiter)
	{
		string word;
		istringstream wordStream(DataCollection);
		while (getline(wordStream, word, delimiter))
			WordList.push_back(word);
	}
};

class Counter
{
public:
	void operator()(map<string, int> *pResultTable, int start)
	{
		while (WordList.size() != input_size);
		for (int i = start; i < start + input_size/no_of_threads; i++)
		{
			int count = (*pResultTable)[string(WordList[i])];
			count = count + 1;
			(*pResultTable)[string(WordList[i])] = count;
		}
	}
};

class WordCounter
{
private:
	map<string, int> ResultTables[no_of_threads];
public:
	void Map()
	{
		thread parser(Parser(), ',');
		parser.join();
		
	}
	void Reduce()
	{
		thread counter1(Counter(), &ResultTables[0], 0);
		thread counter2(Counter(), &ResultTables[1], 3);
		thread counter3(Counter(), &ResultTables[2], 6);
		
		counter1.join();
		counter2.join();
		counter3.join();

		for (int i = 0; i < no_of_threads; i++)
		{
			for (map<string, int>::iterator itr = ResultTables[i].begin();
				itr != ResultTables[i].end(); itr++)
				ResultTable[itr->first] += itr->second;
		}
	}
};

void removespace(string str)
{
    istringstream ss(str);

    string word;

    while (ss >> word)
    {
        cout << word << "\n";
    }
}

//int main()
//{
//
//
//    string temp;
//    std::ifstream file("4727.txt");
//    while (!file.eof())
//    {
//        getline(file, temp);
//        istringstream ss(temp);
//        string word;
//        while (ss >> word)
//        {
//            DataCollection += word;
//            DataCollection += ',';
//
//        }
//
//    }
//    transform(DataCollection.begin(), DataCollection.end(), DataCollection.begin(), ::tolower);
//
//	WordCounter wordcounter;
//
//	wordcounter.Map();
//	wordcounter.Reduce();
//	ofstream myfile;
//	myfile.open("output.txt");
//
//
//	for (map<string, int>::iterator itr = ResultTable.begin(); itr != ResultTable.end(); itr++)
//		myfile << itr->first << "	" << itr->second << endl;
//
//
//	myfile.close();
//
//	return 0;
//}


int main()
{
    char c;
    int iCount[128];
    for (int i=0; i<128;i++)
        iCount[i] = 0;
    string temp;
    long lSize;
    char * buffer;
    size_t result;
    vector<char> array;
    map<char, int> counter;

    FILE *pFile = fopen("4727.txt", "r");
    if ( pFile == NULL )
        cout << "Error opening file" << endl;
    else
    {
        fseek (pFile , 0 , SEEK_END);
        lSize = ftell (pFile);
        rewind (pFile);


        buffer = (char*) malloc (sizeof(char)*lSize);

        result = fread (buffer,1,lSize,pFile);

        for (int i = 0; i < strlen(buffer); i++)
        {
            if (buffer[i] != ' ')
            {
                if (counter.count(buffer[i]) > 0)
                {
                    counter[buffer[i]] += 1;
                }
                else{
                    counter[buffer[i]] = 1;
                }

            }

        }

        for (map<char, int>::iterator it = counter.begin();it!= counter.end(); it ++)
        {
            cout << it -> first << ':' << it-> second << endl;
        }



//        :2
//        :1
//        (:1
//        ):1
//        ,:3
//        -:4
//        .:6
//        0:6
//        1:28
//        2:5
//        3:3
//        5:3
//        6:5
//        7:9
//        8:24
//        9:20
//        A:9
//        B:5
//        C:7
//        D:1
//        E:3
//        G:6
//        H:6
//        I:6
//        N:7
//        O:3
//        P:14
//        R:5
//        S:16
//        T:4
//        U:4
//        W:3
//        Y:1
//        a:246
//        b:43
//        c:125
//        d:166
//        e:443
//        f:85
//        g:81
//        h:125
//        i:286
//        j:2
//        k:27
//        l:161
//        m:77
//        n:243
//        o:243
//        p:75
//        q:2
//        r:209
//        s:219
//        t:332
//        u:80
//        v:34
//        w:44
//        x:22
//        y:52

    }
    fclose(pFile);
    return 0;
}

