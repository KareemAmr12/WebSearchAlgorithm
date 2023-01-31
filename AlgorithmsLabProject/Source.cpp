#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
using namespace std;

struct page
{
	string name;
	float score;
	float ctr;
	int clicks;
	float rank;
	int pageRank;
	vector<string> pointingto;
	vector<string> pointedatby;
	vector<string> keywords;
	int impressions;
};

void Swap(int* xp, int* yp)
{
	int temp = *xp;
	*xp = *yp;
	*yp = temp;
}

vector<page> selection_sort_score(vector<page> pages)
{
	vector<page> temp1;
	int i, j, min_idx;
	int k = 1;
	int temp;
	for (i = 0; i < pages.size() - 1; i++)
	{
		min_idx = i;
		for (j = i + 1; j < pages.size(); j++)
		{
			if (pages[j].score < pages[min_idx].score)
			{
				min_idx = j;
				k++;
			}
		}
		if (min_idx != i)
		{
			iter_swap(pages.begin() + min_idx, pages.begin() + i);
		}
	}
	temp1 = pages;
	return temp1;
}


vector<page> selection_sort_Rank(vector<page> pages)
{
	vector<page> temp1;
	int i, j, min_idx;
	int k = 1;
	int temp;
	for (i = 0; i < pages.size() - 1; i++)
	{
		min_idx = i;
		for (j = i + 1; j < pages.size(); j++)
		{
			if (pages[j].rank < pages[min_idx].rank)
			{
				min_idx = j;
				k++;
			}
		}
		if (min_idx != i)
		{
			iter_swap(pages.begin() + min_idx, pages.begin() + i);
		}
	}
	temp1= pages;
	return temp1;
}

int PNsearch(vector<page> arr, string x)
{
	int n = arr.size();
	int i;
	for (i = 0; i < n; i++)
		if (arr[i].name == x)
			return i;
	return -1;
}

bool Wsearch(vector<page> arr, vector<string> x, int i)
{
		for (int j = 0; j < arr[i].keywords.size(); j++)
		{
			for (int k = 0; k < x.size(); k++)
			{
				if (arr[i].keywords[j] == x[k])
					return true;
			}
		}
	return false;
}

void edit(vector<page> pages)
{
	ofstream impressions;
	impressions.open("Number of impressions file.csv");
	for (int i = 0; i < pages.size(); i++)
	{
		impressions << pages[i].name << ',' << pages[i].impressions << ',' << pages[i].clicks;
		if (i != pages.size()-1)
		{
			impressions << endl;
		}
	}
}

vector<page> PageRank(vector<page> pages)
{
	vector<page> temp;
	for (int i = 0; i < pages.size(); i++)
	{
		pages[i].rank = 1.0f / (float)pages.size();
	}
	vector<float> newRanks;
	float sum = 0.0f;
	int index;
	for (int k = 0; k < 2; k++)
	{
		newRanks.clear();
		for (int i = 0; i < pages.size(); i++)
		{
			sum = 0.0f;
			for (int j = 0; j < pages[i].pointedatby.size(); j++)
			{
				index = PNsearch(pages, pages[i].pointedatby[j]);
				sum = sum + (pages[index].rank / (float)pages[index].pointingto.size());
			}
			newRanks.push_back(sum);
		}
		for (int i = 0; i < pages.size(); i++)
		{
			pages[i].rank = newRanks[i];
		}
	}
	vector<page>::iterator it;
	pages = selection_sort_Rank(pages);
	for (int i = 0; i < pages.size(); i++)
	{
		pages[i].pageRank = i + 1;
	}
	temp = pages;
	return temp;
}
vector<page> CTR(vector<page> pages)
{
	vector<page> temp;
	for (int i = 0; i < pages.size(); i++)
	{
		pages[i].ctr = ((float)pages[i].clicks / (float)pages[i].impressions) * 100.0f;
	}
	temp = pages;
	return temp;
}

vector<page> Score(vector<page> pages)
{
	vector<page> temp;
	pages = PageRank(pages);
	pages = CTR(pages);
	for (int i = 0; i < pages.size(); i++)
	{
		pages[i].score = (0.4 * (float)pages[i].pageRank) +
			(((float)1 - ((0.1 * (float)pages[i].impressions) / ((float)1 + 0.1 * (float)pages[i].impressions))) * (float)pages[i].pageRank
				+ ((0.1 * (float)pages[i].impressions) / ((float)1 + 0.1 * (float)pages[i].impressions)) * pages[i].ctr) * 0.6;
	}
	temp = pages;
	return temp;
}

vector<page> initialize()
{
	vector<page> pages;
	string link1, link2;
	string name;

	ifstream keywords;
	keywords.open("Keyword file.csv");
	
	int i = 0;
	while (keywords.is_open() && !keywords.eof())
	{
		name.clear();
		pages.resize(pages.size() + 1);
		getline(keywords, link1, '\n');
		int index = link1.find_first_of(',');
		name = link1.substr(0, index);
		pages[i].name = name;
		link1 = link1.substr(index + 1);
		while (link1 != "")
		{
			index = link1.find_first_of(',');
			name = link1.substr(0, index);
			for (int i = 0; i < name.size(); i++)
			{
				name[i] = toupper(name[i]);
			}
			pages[i].keywords.push_back(name);
			
			link1 = link1.substr(index + 1);
			if (index == string::npos)
				break;
		}
		i++;
	}

	ifstream webgraph;
	webgraph.open("Web graph file.csv");

	while (webgraph.is_open() && !webgraph.eof())
	{
		getline(webgraph, link1, ',');
		getline(webgraph, link2, '\n');
		
		i = PNsearch(pages, link1);
		pages[i].pointingto.push_back(link2);
		i = PNsearch(pages, link2);
		pages[i].pointedatby.push_back(link1);
	}

	ifstream impressions;
	impressions.open("Number of impressions file.csv");

	int pi;
	int ImNumber;
	int ImNumber2;
	int count = 0;
	
	while (impressions.is_open() && !impressions.eof())
	{
		name.clear();
		getline(impressions, link1, '\n');
		int index = link1.find_first_of(',');
		name = link1.substr(0, index);
		pi = PNsearch(pages, name);
		link1 = link1.substr(index + 1);
		ImNumber = stoi(link1.substr(0, index));
		index = link1.find_first_of(',');
		link1 = link1.substr(index + 1);
		ImNumber2 = stoi(link1.substr(0, index));
		pages[pi].impressions = ImNumber;
		pages[pi].clicks = ImNumber2;
		index = link1.find_first_of(',');
	}
	pages = Score(pages);
	return pages;
}

vector<page> srch(string keywords, vector<page> pages)
{
	vector<string> quotations;
	vector<string> AND;
	vector<string> CAND;
	vector<string> OR;
	vector<page> result;
	string::iterator it;
	int checkand = -1;
	int checkor = -1;
	int checkq = -1;
	

	for (int i = 0; i < keywords.size(); i++)
	{
		if (keywords[i] != '\"' && keywords[i] != ' ')
		{
			keywords[i] = toupper(keywords[i]);
		}
	}
	string temp = keywords;
	checkor = temp.find("OR");
	checkand = temp.find("AND");
	checkq = temp.find('\"');

	while (temp != "" && checkor != -1 && checkand == -1)
	{
		int index = temp.find("OR");
		string name = temp.substr(0, index - 1);
		for (int i = 0; i < name.size(); i++)
		{
			name[i] = toupper(name[i]);
		}
		OR.push_back(name);
		temp = temp.substr(index + 3);
		if (index == string::npos)
			break;
	}
	temp = keywords;
	while (temp != "" && checkand != -1 && checkor == -1)
	{
		int index = temp.find("AND");
		string name = temp.substr(0, index - 1);
		for (int i = 0; i < name.size(); i++)
		{
			name[i] = toupper(name[i]);
		}
		AND.push_back(name);
		temp = temp.substr(index + 4);
		if (index == string::npos)
			break;
	}
	temp = keywords;
	int doublecheckand;
	int doublecheckor;
	bool special = false;
	while (temp != "" && checkand != -1 && checkor != -1 && checkq == -1)
	{
		doublecheckand = temp.find("AND");
		doublecheckor = temp.find("OR");
		if (doublecheckand < doublecheckor)
		{
			int index = temp.find("OR");
			string name = temp.substr(0, index - 1);
			
			while (temp != "" && doublecheckand != -1)
			{
				doublecheckand = name.find("AND");
				string name2 = name.substr(0, doublecheckand - 1);

				AND.push_back(name2);
				name = name.substr(doublecheckand + 4);
				if (doublecheckand == string::npos)
					break;
			}
			name = temp.substr(index + 3);
			OR.push_back(name);
			temp = temp.substr(index + 3);
			index = temp.find("OR");
			if (index == string::npos)
				break;
		}
		if (doublecheckand > doublecheckor)
		{
			special = true;
			int index = temp.find("AND");
			string name = temp.substr(0, index - 1);

			while (temp != "" && doublecheckor != -1)
			{
				doublecheckor = name.find("OR");
				string name2 = name.substr(0, doublecheckor - 1);

				CAND.push_back(name2);
				name = name.substr(doublecheckor + 3);
				if (doublecheckor == string::npos)
					break;
			}
			name = temp.substr(index + 4);
			AND.push_back(name);
			temp = temp.substr(index + 4);
			index = temp.find("AND");
			if (index == string::npos)
				break;
		}
	}
	if (special == false)
	{
		temp = keywords;
		while (temp != "" && checkq != -1)
		{
			int index = temp.find('\"');
			if (index != -1)
			{
				it = temp.begin();
				if (index == 0)
				{
					temp.erase(it);
				}
				else
				{
					temp.erase(it, it + index);
				}
				index = temp.find('\"');
				string name = temp.substr(0, index);
				for (int i = 0; i < name.size(); i++)
				{
					name[i] = toupper(name[i]);
				}
				quotations.push_back(name);
				temp = temp.substr(index + 1);
				if (index == string::npos)
					break;
			}
			checkq = -1;
		}

		for (int i = 0; i < OR.size(); i++)
		{
			if (OR[i][0] == '\"')
			{
				int index = OR[i].find('\"');
				if (index != -1)
				{
					it = OR[i].begin();
					if (index == 0)
					{
						OR[i].erase(it);
					}
					else
					{
						OR[i].erase(it, it + index);
					}
					index = OR[i].find('\"');
					string name = OR[i].substr(0, index);
					quotations.push_back(name);
				}
			}
		}

		for (int i = 0; i < AND.size(); i++)
		{
			if (AND[i][0] == '\"')
			{
				int index = AND[i].find('\"');
				if (index != -1)
				{
					it = AND[i].begin();
					if (index == 0)
					{
						AND[i].erase(it);
					}
					else
					{
						AND[i].erase(it, it + index);
					}
					index = AND[i].find('\"');
					string name = AND[i].substr(0, index);

					AND.erase(AND.begin() + i);

					if (Wsearch(pages, OR, i))
					{
						result.push_back(pages[i]);
					}
					AND.push_back(name);
					quotations.clear();
				}
			}
		}

		if (checkand == -1 && checkor == -1 && checkq == -1)
		{
			while (temp != "")
			{
				int index = temp.find(" ");
				string name = temp.substr(0, index);
				for (int i = 0; i < name.size(); i++)
				{
					name[i] = toupper(name[i]);
				}
				OR.push_back(name);
				temp = temp.substr(index + 1);
				if (index == string::npos)
					break;
			}
		}
		for (int i = 0; i < pages.size(); i++)
		{
			if (Wsearch(pages, quotations, i))
			{
				result.push_back(pages[i]);
			}
			if (Wsearch(pages, OR, i))
			{
				result.push_back(pages[i]);
			}
		}


		if (checkand != -1)
		{
			bool check = true;
			for (int i = 0; i < pages.size(); i++)
			{
				check = true;
				for (int j = 0; j < AND.size(); j++)
				{
					if (check == false)
					{
						break;
					}
					int cnt = 0;
					for (int k = 0; k < pages[i].keywords.size(); k++)
					{
						if (pages[i].keywords[k] == AND[j])
						{
							break;
						}
						else
						{
							cnt++;
						}
						if (cnt == pages[i].keywords.size())
						{
							check = false;
							break;
						}
					}
				}
				if (check == true)
				{
					result.push_back(pages[i]);
				}
			}
		}
	}
	else
	{
	for (int i = 0; i < pages.size(); i++)
	{
		if (Wsearch(pages, CAND, i) && Wsearch(pages, AND, i))
		{
			result.push_back(pages[i]);
		}
	}
	}

	if (!result.empty())
	{
		result = selection_sort_score(result);
		for (int i = 1; i < result.size(); i++)
		{
			if (result[i-1].name == result[i].name)
			{
				result.erase(result.begin() + i);
			}
		}
	}
		return result;
	}


void search3(vector<page> result, vector<page> pages);

void search2(vector<page> pages)
{
	system("CLS");
	string s;
	int c;
	int c2;
	int index;
	bool nvalid = true;
	string search;
	vector<page> result;
	cout << "Search: ";
	cin.ignore();
	getline(cin, search);
	result = srch(search, pages);
	reverse(result.begin(), result.end());
	if (!result.empty())
	{
		cout << "\nSearch results: \n";
		for (int i = 0; i < result.size(); i++)
		{
				cout << i + 1 << ".  " << result[i].name << endl;
				index = PNsearch(pages, result[i].name);
				pages[index].impressions++;
			
		}
		cout << "\nWould you like to\n";
		cout << "1. Choose a webpage to open\n2.  New search\n3.  Exit\n\n";
		cout << "Type in your choice: ";
		cin >> s;
		while (s.length() > 1 || s < "1" || s > "3")
		{
			s.clear();
			cout << "Invalid!" << endl << "Please enter a valid choice: ";
			cin >> s;
			cout << endl;
		}
		c = stoi(s);
		while (nvalid)
		{
			if (c == 1)
			{
				nvalid = false;
				search3(result, pages);
			}
			if (c == 2)
			{
				nvalid = false;
				search2(pages);
			}
			if (c == 3)
			{
				nvalid = false;
				cout << "\nThank you for choosing our search engine." << endl;
				edit(pages);
				exit(0);
			}
			else
			{
				nvalid = true;
				cout << "Please enter a valid choice: ";
				cin >> c;
			}
		}
	}
	if(result.empty())
	{
		cout << "No results found.\n";
		cout << "Would you like to\n";
		cout << "\n1.  New search\n2.  Exit\n\n";
		cout << "Type in your choice: ";
		cin >> s;
		while (s.length() > 1 || s < "1" || s > "2")
		{
			s.clear();
			cout << "Invalid!" << endl << "Please enter a valid choice: ";
			cin >> s;
			cout << endl;
		}
		c = stoi(s);
		while (nvalid)
		{		
			if (c == 1)
			{
				nvalid = false;
				search2(pages);
			}
			if (c == 2)
			{
				nvalid = false;
				cout << "\nThank you for choosing our search engine." << endl;
				edit(pages);
				exit(0);
			}
			else
			{
				nvalid = true;
				cout << "Please enter a valid choice: ";
				cin >> c;
			}
		}
	}
}
void search4(vector<page> result, vector<page> pages);

void search3(vector<page> result, vector<page> pages)
{
	cout << endl;
	string c;
	int s;
	int index;
	bool nvalid = true;
	cout << "\nSelect a webpage to open: ";
	cin >> c;
	while (c.length() > 1 || c < "1" || c > to_string(result.size()))
	{
		c.clear();
		cout << "Invalid!" << endl << "Please enter a valid choice: ";
		cin >> c;
		cout << endl;
	}
	s = stoi(c);
	index = PNsearch(pages, result[s-1].name);
	pages[index].clicks++;
	
	cout << "You are now viewing " << result[s - 1].name << ".\n";
	cout << "\nWould you like to \n";
	cout << "1. Back to search results\n2.  New search\n3.  Exit\n\n";
	cout << "Type in your choice: ";
	cin >> s;
	while (nvalid)
	{
		if (s == 1)
		{
			nvalid = false;
			search4(result, pages);
		}
		if (s == 2)
		{
			nvalid = false;
			search2(pages);
		}
		if (s == 3)
		{
			nvalid = false;
			cout << "\nThank you for choosing our search engine." << endl;
			edit(pages);
			exit(0);
		}
		else
		{
			nvalid = true;
			cout << "Please enter a valid choice: ";
			cin >> c;
		}
	}
}

void search4(vector<page> result, vector<page> pages)
{
	cout << endl;
	int c;
	string s;
	bool nvalid = true;
	cout << "Search results: \n";
	for (int i = 0; i < result.size(); i++)
	{
		cout << i + 1 << ".  " << result[i].name << endl;
	}
	cout << "\nWould you like to\n";
	cout << "1. Choose a webpage to open\n2.  New search\n3.  Exit\n\n";
	cout << "Type in your choice: ";
	cin >> s;
	while (s.length() > 1 || s < "1" || s > "3")
	{
		s.clear();
		cout << "Invalid!" << endl << "Please enter a valid choice: ";
		cin >> s;
		cout << endl;
	}
	c = stoi(s);
	while (nvalid)
	{
		if (c == 1)
		{
			nvalid = false;
			search3(result, pages);
		}
		if (c == 2)
		{
			nvalid = false;
			search2(pages);
		}
		if (c == 3)
		{
			nvalid = false;
			cout << "\nThank you for choosing our search engine." << endl;
			edit(pages);
			exit(0);
		}
		else
		{
			nvalid = true;
			cout << "Please enter a valid choice: ";
			cin >> c;
		}
	}
}


void menu()
{
	string s;
	int c;
	int c2;
	string search;
	vector<page> result;
	vector<page> pages;
	pages = initialize();
	bool finished = false;
	cout << "Welcome to the search engine!" << endl;
	cout << "\nWhat would you like to do?" << endl;
	cout << "1. New search" << endl << "2. Exit" << endl << endl;
	cout << "Type in your choice: ";
	cin >> s;
	while (s.length() > 1 || s < "1" || s > "2")
	{
		s.clear();
		cout << "Invalid!" << endl << "Please enter a valid choice: ";
		cin >> s;
		cout << endl;
	}
	c = stoi(s);
	while(!finished)
	{
		if (c == 1)
		{
			search2(pages);
		}
		if (c == 2)
		{
			cout << "\nThank you for choosing our search engine." << endl;
			finished = true;
			edit(pages);
			exit(0);
		}
		else
		{
			cout << "Please enter a valid choice: ";
			cin >> c;		
		}
	}
}


int main()
{
	menu();
	system("pause");
	return 0;
}



