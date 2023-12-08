#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include<vector>
using namespace std;
short CntIDauthor = 0,CntIDbook = 0,CntIdSec,CntIdLL;//, CntNameLL = 0 , CntNameSec = 0 , CntGpaSec = 0 , CntGpaLL=0

struct SecIndex
{
    map<string , vector<string>> mp ;// map<AuthorID , vector<ISBN>>
    map<string , vector<string>> mp2 ; //  map<AuthorName , vector<AuthorID>>
};


void printAuthor(char id[]) ;

void buildSecIndex(const std::string& bookDataFile, SecIndex& secIndex) {
    std::fstream bookFile(bookDataFile, std::ios::in | std::ios::binary);

    if (!bookFile) {
        std::cerr << "Error opening BookDataFile." << std::endl;
        return;
    }

    int recordLength;
    Book book;

    while (bookFile.read(reinterpret_cast<char*>(&recordLength), sizeof(recordLength)) &&
           bookFile.read(book.ISBN, sizeof(book.ISBN)) &&
           bookFile.read(book.BookTitle, sizeof(book.BookTitle)) &&
           bookFile.read(book.AuthorID, sizeof(book.AuthorID))) {

        // Convert char arrays to strings
        string authorID(book.AuthorID);
        string isbn(book.ISBN);

        // Update the secondary index
        secIndex.mp[authorID].push_back(isbn);

        // Move to the next record
        bookFile.seekg(recordLength - sizeof(recordLength) - sizeof(char), std::ios::cur);
    }

    bookFile.close();
}

// BOOK UPDATE

short BinarySearchIdbook(char isbn[]) // binary search in the primary index
{
    fstream prim("PIbooks.txt", ios::in | ios::binary | ios::out);
    short first = 0;
    short last = CntIDbook - 1;
    //cout<<"CntIDbook: "<<CntIDbook;
    short mid;
    bool found = false;
    char temp[15];
    string ISBN(isbn);
    while (first <= last && !found)
    {

        mid = (first + last) / 2;
        prim.seekg(mid * 17, ios::beg);
        prim.read(temp, 15);

        if (temp == ISBN)
            found = true;
        else if (temp >  ISBN)
            last = mid - 1;
        else
            first = mid + 1;
    }
    if (found)
    {
        short of;
        prim.seekg((mid * 17) + 15, ios::beg);
        prim.read((char*)&of, sizeof(of));
        prim.close();
        return of;
    }
    else
    {
        short notFound=-1;
        prim.close();
        return notFound;

    }

}

int getTitleSizeBook(fstream& booksFile, short recordOffset){
    booksFile.seekg(recordOffset + sizeof(short));  // Skip the record size and the '|'
    int titleSize = 0;
    char ch;
    while (booksFile.get(ch) && ch != '|') {
        titleSize++;
    }
    return titleSize;
}

void updateBookTitle(string BdataFile, string Pbook, char ISBN [], char newTitle[]) {

    fstream booksPrimaryIndex(Pbook , ios::in | ios::out | ios::binary) ;
    fstream booksFile(BdataFile ,ios::in | ios::out | ios::binary ) ;

    short offset = BinarySearchIdbook(ISBN);
    cout << "Offset: " << offset << endl;

    if (offset == -1) {
        cout << "Book with ISBN " << ISBN << " is not found" << endl;
        return;
    }

    int oldTitleSize = getTitleSizeBook(booksFile, offset + strlen(ISBN)+1);
    cout << "Old Title Size: " << oldTitleSize << endl;
    booksFile.seekp(offset + strlen(ISBN)+1+ sizeof(short), ios::beg);

    if (strlen(newTitle) < oldTitleSize) {
        string paddedTitle = newTitle + string(oldTitleSize - strlen(newTitle) , ' ');
        booksFile.write(paddedTitle.c_str(), oldTitleSize);
    } else if (strlen(newTitle) == oldTitleSize) {
        booksFile.write(newTitle, oldTitleSize);
    } else {
        cout << "New Title is bigger than the old one, please enter smaller than or equal to the old one!" << endl;
    }
    booksFile.close();
    booksPrimaryIndex.close();
}


// __________________________________________________________________________



// AUTHOR UPDATE
short BinarySearchIdauthor(char ID[]) // binary search in the primary index
{
    fstream prim("PIAuthors.txt", ios::in | ios::binary | ios::out);
    short first = 0;
    short last = CntIDbook - 1;
    //cout<<"CntIDbook: "<<CntIDbook;
    short mid;
    bool found = false;
    char temp[15];
    string AuthorID(ID);
    while (first <= last && !found)
    {

        mid = (first + last) / 2; // cout<<"mid"<<mid;
        prim.seekg(mid * 17, ios::beg);
        prim.read(temp, 15);
        //cout<<temp;
        if (temp == AuthorID)
            found = true;
        else if (temp >  AuthorID)
            last = mid - 1;
        else
            first = mid + 1;
    }
    if (found)
    {
        short of;
        prim.seekg((mid * 17) + 15, ios::beg);
        prim.read((char*)&of, sizeof(of));
        prim.close();
        return of;
    }
    else
    {
        short notFound=-1;
        prim.close();
        return notFound;

    }

}

int getNameSizeAuthor(fstream& authorsFile, short recordOffset) {
    authorsFile.seekg(recordOffset + sizeof (short));  // Skip the record size and the '|'
    int nameSize = 0;
    char ch;
    while (authorsFile.get(ch) && ch != '|') {
        nameSize++;
    }
    return nameSize;
}


void updateAuthorName(string ADataFile, string Pauthor, char authorID[], char newName[]) {
    fstream authorsPrimaryIndex(Pauthor , ios::in | ios::out| ios::binary ) ;
    fstream authorsFile(ADataFile , ios::in | ios::out| ios::binary);

    short offset = BinarySearchIdauthor(authorID);
    if (offset == -1) {
        cout << "**Author with ID " << authorID << " is not found.**" << endl;
        return;
    }

    int oldNameSize = getNameSizeAuthor(authorsFile, offset + strlen(authorID) + 1);
    cout << "Old Name Size: " << oldNameSize << endl;
    authorsFile.seekp(offset + strlen(authorID) + 1 + sizeof(short), ios::beg);


    if (strlen(newName) < oldNameSize) {
        string paddedName = newName + std::string(oldNameSize - strlen(newName), ' ');
        authorsFile.write(paddedName.c_str(), oldNameSize);
    } else if (strlen(newName) == oldNameSize) {
        authorsFile.write(newName, oldNameSize);
    } else {
        cout << "New Author Name is bigger than the old one, please enter smaller than or equal to the old one!" << endl;
    }

    authorsFile.close();
    authorsPrimaryIndex.close();
}


void printAuthor(char id[] , string Authorfile){
    short off=BinarySearchIdauthor(id);
    if(off<0){
        cout<<"no such ID\n";

    }
    else{
        fstream file(Authorfile, ios::out |ios::in | ios::binary | ios::app);
        string l="";
        file.seekg(off,ios::beg);
        char c;
        file.read((char*)&c,sizeof(c));
        while(c!='|'){
            l+=c;
            file.get(c);
        }
        int length=stoi(l);
        string buf;
        file.getline((char*)&buf,length);
        string record(buf);
        //cout<<record;
        int pos=record.find('|');
        string  ID,name,address;
        ID=record.substr(0,pos);
        cout<<"AUTHOR'S ID : \t"<<ID<<endl;
        record.erase(0,ID.length()+1);
        pos=record.find('|');
        name=record.substr(0,pos);
        cout<<"AUTHOR'S NAME : \t"<<name<<endl;
        record.erase(0,name.length()+1);
        pos=record.find('|');
        address=record.substr(0,pos);
        cout<<"AUTHOR'S ADDRESS : \t"<<address<<endl;
        file.close();
    }

}




void printbook(char isbn[]){
    short off=BinarySearchIdbook(isbn);
    //cout<<off;
    if(off<0){
        cout<<"no such ISBN\n";


    }
    else{
        fstream file("books.txt", ios::out |ios::in | ios::binary | ios::app);
        string l="";
        file.seekg(off,ios::beg);
        char c;
        file.read((char*)&c,sizeof(c));
        while(c!='|'){
            l+=c;
            file.get(c);
        }
        int length=stoi(l);
        string buf;
        file.getline((char*)&buf,length);
        string record(buf);
        //cout<<record;
        int pos=record.find('|');
        string  ID,name,address;
        ID=record.substr(0,pos);
        cout<<"ISBN : \t"<<ID<<endl;
        record.erase(0,ID.length()+1);
        pos=record.find('|');
        name=record.substr(0,pos);
        cout<<"TITLE : \t"<<name<<endl;
        record.erase(0,name.length()+1);
        pos=record.find('|');
        address=record.substr(0,pos);
        cout<<"AUTHOR'S ID : \t"<<address<<endl;
        file.close();
    }

}


// Queries
void selectAuthorsPrimary(char id[] , string Authorsfile)
{
    printAuthor(id , Authorsfile);
}


void selectAuthorNAME(char id[] , string Authorsfile)
{
    short offset = BinarySearchIdauthor(id);
    if (offset == -1) {
        cout << "**Author with ID " << id << " is not found.**" << endl;
        return;
    }

    fstream authorsData(Authorsfile, std::ios::in | std::ios::binary | ios::out);

    char authorName[30];
    authorsData.seekg(offset + strlen(id) + 1 , ios::beg);
    authorsData.read(authorName, sizeof(authorName));
    cout << "Author Name: " << authorName << std::endl;

}




void selectBooksForAuthor(const SecIndex& secIndex, const string& authorID) {
    auto it = secIndex.mp.find(authorID);
    if (it != secIndex.mp.end()) {
        for(auto i : it->second){
            char isbn[i.size()+1] ;
            strcpy(isbn,i.c_str());
            printbook(isbn);
            return;
        }
    }
    cout << "author not found" << endl ;
}



void shiftd(char aid[15],short mid);
void printbook(char isbn[] );
void insertLL(char isbn[])
{
	fstream  idsec2("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
	idsec2.seekp(0, ios::beg);
	vector<short>avail;
	if (avail.empty())
	{
		avail.push_back(-1);
		idsec2.write((char*)&avail[avail.size() - 1], sizeof(avail[avail.size() - 1]));
	}
	idsec2.close();
	short header;
	idsec2.open("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
	idsec2.seekg(0, ios::beg);
	idsec2.read((char*)&header, sizeof(header));
	idsec2.close();
	cout << "header: " << header << endl;
	bool flag = true;
	short insertionOffset;//for the pointer that points to the new inserted record in ll list 
	if (header != -1)//avail list is not empty
	{// 5 8 
		flag = false;
		idsec2.open("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
		idsec2.seekp(header, ios::end);//0
	
		idsec2.write(isbn, 15);
		short nega = -1;
		idsec2.write((char*)&nega, sizeof(nega));
		insertionOffset = header;
		idsec2.close();
	
		avail.pop_back();
		idsec2.open("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
		idsec2.seekp(0, ios::beg);
		idsec2.write((char*)&avail[avail.size() - 1], sizeof(avail[avail.size() - 1]));
		idsec2.close();
	}
}
void insertsortedAidSec(char aid[],char isbn[])
{
	fstream idsec("secAthrId.txt",ios::out|ios::in|ios::binary);
	fstream  idsec2("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
	//idsec.open("secAthrId.txt",ios::out|ios::in|ios::binary);
	//idsec2.open("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
	streampos end,begin,lend,lbeg;
	 if (idsec.bad())
        {
            cout << "failed to open sec1" << endl;
            return ;
        }
	idsec.seekg(0,ios::beg);
	begin=idsec.tellg();
	idsec.seekg(0,ios::end);
	end=idsec.tellg();
	idsec.close();
	idsec2.seekg(0,ios::beg);
	lbeg=idsec2.tellg();
	idsec2.seekg(0,ios::end);
	lend=idsec2.tellg();
	idsec2.close();
	CntIdSec=(end-begin)/17;
	CntIdLL=(lend-lbeg)/17;
	cout<<"file count = "<<CntIdSec<<endl;
	cout<<"file count LL = "<<CntIdLL<<endl;
	short first = 0;
	short last = CntIdSec - 1;
	short mid;
	bool found = false;
	char temp[15];
	idsec.open("secAthrId.txt",ios::out|ios::in|ios::binary);
	while (first <= last && !found)
	{
		mid = (first + last) / 2;
		idsec.seekg(mid * 17, ios::beg);
		idsec.read((char*)&temp, sizeof(temp));

		if (strcmp(temp, aid) == 0)
			found = true;
		else if (strcmp(temp, aid) == 1)
			last = mid - 1;
		else
			first = mid + 1;
	}
	idsec.close();
	
	if(!found)//didnot find the id in the secondary index file
	{
		cout<<"didnot find the id in the secondary index file"<<endl;
		if(CntIdSec==0)//sec index file is empty
		{
			cout<<"sec index file is empty"<<endl;
			idsec.open("secAthrId.txt",ios::out|ios::in|ios::binary);
			idsec.seekg(0,ios::beg);
			idsec.write(aid,15);
			idsec.write((char*)&CntIdLL,sizeof(CntIdLL));	
            idsec2.open("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
			if (idsec2.fail())
			{
                cout << "failed to open3" << endl;
            return ;
            }
			idsec2.seekg(0,ios::beg);
			idsec2.write(isbn,15);
			short nega=-1;
			idsec2.write((char*)&nega,sizeof(nega));
			//CntIdLL++;
			//CntIdSec++;
			idsec2.close();
			idsec.close();
			idsec2.open("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
			idsec2.seekg(0,ios::beg);
	        lbeg=idsec2.tellg();
	        idsec2.seekg(0,ios::end);
	        lend=idsec2.tellg();
	        idsec2.close();
	        CntIdLL=(lend-lbeg)/17;
			cout<<"ll file count"<<CntIdLL<<endl;
		}
		else//not found but file is not empty
		{
			cout<<"not found but file is not empty"<<endl;
			idsec.open("secAthrId.txt",ios::out|ios::in|ios::binary);
			short offplace = -1;
			int i=0;
			idsec.seekg(0,ios::beg);
			char temp2[15];
			while(i<CntIdSec)
			{
				idsec.read(temp2,15);
				if(strcmp(aid,temp2)==-1)
				{
					offplace=idsec.tellg();
					offplace-=15;
					break;
				}
				else{
					idsec.seekg(2,ios::cur);
				}
				i++;
			}
			idsec.close();
			if(offplace==-1)//the new secondary key is bigger than existing keys
			{
				cout<<"the new secondary key is bigger than existing keys"<<endl;
				idsec.open("secAthrId.txt",ios::out|ios::in|ios::binary);
				idsec.seekg(CntIdSec*17,ios::beg);
				idsec.write(aid,15);
				idsec.write((char*)&CntIdLL,sizeof(CntIdLL));
				idsec2.open("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
				idsec2.seekg(0,ios::end);
				idsec2.write(isbn,15);
				short nega=-1;
				idsec2.write((char*)&nega,sizeof(nega));
				idsec2.close();
				idsec.close();
				//CntIdLL++;
			    //CntIdSec++;
			}
			else//it is not the biggest so we have to start shifting
			{
				cout<<"it is not the biggest so we have to start shifting"<<endl;
				idsec.open("secAthrId.txt",ios::out|ios::in|ios::binary);
				char idtemp[15];
				short idoffset;
				idsec.seekg((CntIdSec-1)*17,ios::beg);
				idsec.read(idtemp,15);
				idsec.read((char*)&idoffset,sizeof(idoffset));
				char temp1[15],tmp2[15];
				short ofstmp1,ofstmp2;
				idsec.seekg(offplace,ios::beg);
				idsec.read(temp1,15);
				idsec.read((char*)&ofstmp1,sizeof(ofstmp1));
				int i=0;
				while (i<CntIdSec-1)
				{
					idsec.read(tmp2,15);
					idsec.read((char*)&ofstmp2,sizeof(ofstmp2));
					int shift=idsec.tellg();
					idsec.seekp(shift-17);
					idsec.write(temp1,15);
					idsec.write((char*)&ofstmp1,sizeof(ofstmp1));
					ofstmp1=ofstmp2;
					int j;
					for( j=0;tmp2[j]!='\0';j++)
					{
						temp1[j]=tmp2[j];
					}
					temp1[j]='\0';
					i++;
				}
				
				idsec.close();
				idsec.open("secAthrId.txt",ios::out|ios::in|ios::binary);
				idsec.seekp(offplace,ios::beg);
				idsec.write(aid,15);
				idsec.write((char*)&CntIdLL,sizeof(CntIdLL));
				idsec.seekp(CntIdSec*17,ios::beg);
				idsec.write(idtemp,15);
				idsec.write((char*)&idoffset,sizeof(idoffset));
				idsec2.open("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
				idsec2.seekg(0,ios::end);
				idsec2.write(isbn,15);
				short nega=-1;
				idsec2.write((char*)&nega,sizeof(nega));
				idsec2.close();
				idsec.close();
				//CntIdLL++;
			    //CntIdSec++;

			}


		}

	}
	else// they secindary key exist before
	{
		short offInd;
		idsec.open("secAthrId.txt",ios::out|ios::in|ios::binary);
		idsec.seekp((mid*17)+15,ios::beg);
		idsec.read((char*)&offInd,sizeof(offInd));
		idsec.close();
		cout<<" offset ="<<offInd<<endl;
		idsec2.open("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
		idsec2.seekg((offInd*17)+15,ios::beg);
		short next=-2;
		while (true)
		{
			idsec2.read((char*)&next,sizeof(next));
			cout<<" LL ="<<next<<endl;
			if (next==-1)
			{
				cout<<" entered -1"<<endl;
				/*int p=idsec.tellg();
				idsec2.seekp(p-2,ios::beg);
				cout<<" cnt in ll ="<<CntIdLL<<endl;
				idsec2.write((char*)&CntIdLL,sizeof(CntIdLL));
				idsec2.seekp(0,ios::end);
				idsec2.write(isbn,15);
				short t=-1;
				idsec2.write((char*)&t,2);*/
				idsec2.seekg(-2,ios::cur);
				idsec2.write((char*)&CntIdLL,sizeof(CntIdLL));
				idsec2.seekg(0,ios::end);
				idsec2.write(isbn,15);
				short t=-1;
				idsec2.write((char*)&t,2);
				break;

			}
			else
			{
				cout<<" entered -1 else "<<endl;
				idsec2.seekg((next*17)+15,ios::beg);
			}
			

		}
		idsec2.close();

		idsec2.open("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
	   char idtemp[15];
	   short idoffset;
	   idsec2.read(idtemp,15);
	   idsec2.read((char*)&idoffset,sizeof(idoffset)); 
	

	}
	idsec.close();
	idsec2.close();
	idsec.open("secAthrId.txt",ios::out|ios::in|ios::binary);
	//idsec2.open("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
	char idtemp[15];
	short idoffset;


	idsec.read(idtemp,15);
	idsec.read((char*)&idoffset,sizeof(idoffset));
	while ((idsec.good()))
	{
		cout<<"aid in secondary ="<<idtemp<<" "<<"off ="<<idoffset<<endl;
		idsec.read(idtemp,15);
	    idsec.read((char*)&idoffset,sizeof(idoffset));
	}
	idsec.close();
	idsec2.open("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
	idsec2.read(idtemp,15);
	idsec2.read((char*)&idoffset,sizeof(idoffset)); 
    int i=0;
    while (idsec2.good())
	{
		cout<<"isbn in secondary ="<<idtemp<<" "<<"next ="<<idoffset<<endl;
		idsec2.read(idtemp,15);
	    idsec2.read((char*)&idoffset,sizeof(idoffset));
		i++;
	}



} 
void searchBySecId(char aid[15])
{
	fstream file;
	file.open("secAthrId.txt",ios::out|ios::in|ios::binary);
	streampos end,begin,lend,lbeg;
	file.seekg(0,ios::beg);
	begin=file.tellg();
	file.seekg(0,ios::end);
	end=file.tellg();
	file.close();
	CntIdSec=(end-begin)/17;
	short first = 0;
	short last = CntIdSec - 1;
	short mid;
	bool found = false;
	char temp[15];
	
	file.open("secAthrId.txt",ios::out|ios::in|ios::binary);
	while (first <= last && !found)
	{
		mid = (first + last) / 2;
		file.seekg(mid * 17, ios::beg);
		file.read((char*)&temp, sizeof(temp));

		if (strcmp(temp, aid) == 0)
			found = true;
		else if (strcmp(temp, aid) == 1)
			last = mid - 1;
		else
			first = mid + 1;
	}
	file.close();
	if(!found)
	{
		cout<<"author id doesnot exist"<<endl;
		return;
	}
	else
	{
		short address;
		short offset;
        char tmp[15];
        file.open("secAthrId.txt",ios::out|ios::in|ios::binary);
		file.seekg((mid*17),ios::beg);
        file.read(tmp,15);
		file.read((char*)&offset,sizeof(offset));
		file.close();
        cout<<"sec key =>"<<tmp<<endl;
		cout<<"offset of primary key :"<<offset<<endl;
		fstream idll;
		idll.open("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
		idll.seekg(offset*17,ios::beg);
		char isbn[15];
		short next;
		idll.read(isbn,sizeof(isbn));
		idll.read((char*)&next,sizeof(next));
		cout<<"isbn : "<<isbn<<"  offset :"<<next<<endl;
		//address=BinarySearchIdbook(isbn);
		printbook((char*)&isbn);
		/*fstream books;
		books.open("books.txt",ios::out|ios::in|ios::binary);
		books.seekg(address,ios::beg);*/
		while(next!=-1)
		{   cout<<"entered -1"<<endl;
            idll.seekg(((next)*17) ,ios::beg);
			idll.read(isbn,15);
		    idll.read((char*)&next,sizeof(next));
		    cout<<"isbn : "<<isbn<<"offset :"<<next<<endl;
            
		   // address=BinarySearchIdbook(isbn);
		    printbook(isbn);
		}



	}

}
void deleteIsbnS(char aid[],char isbn[])//delete primary key in secodary index after deleting a book
{
   fstream idsec;
	fstream idsec2;
	streampos end,beg,lend,lbeg;
	//idsec.open("secAthrId.txt",ios::out|ios::in|ios::binary);
	/*idsec2.open("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
	streampos end,begin,lend,lbeg;
	if (idsec2.bad())
        {
            cout << "failed to open sec1" << endl;
            return ;
        }
	lbeg=idsec2.tellg();
	idsec2.seekg(0,ios::end);
	lend=idsec2.tellg();
	idsec2.close();
	CntIdLL=(lend-lbeg)/17;
	short first = 0;
	short last = CntIdLL - 1;
	short mid;
	bool found = false;
	char temp[15];
	idsec2.open("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
	while (first <= last && !found)
	{
		mid = (first + last) / 2;
		idsec2.seekg(mid * 17, ios::beg);
		idsec2.read((char*)&temp, sizeof(temp));

		if (strcmp(temp, isbn) == 0)
			found = true;
		else if (strcmp(temp, isbn) == 1)
			last = mid - 1;
		else
			first = mid + 1;
	}
	idsec2.close();
*/
	idsec.open("secAthrId.txt",ios::out|ios::in|ios::binary);
	 if (idsec.bad())
        {
            cout << "failed to open sec1" << endl;
            return ;
        }
	idsec.seekg(0,ios::beg);
	beg=idsec.tellg();
	idsec.seekg(0,ios::end);
	end=idsec.tellg();
	idsec.close();
	CntIdSec=(end-beg)/17;
	cout<<"file count = "<<CntIdSec<<endl;
	
	short first2 = 0;
	short last2 = CntIdSec - 1;
	short mid2;
	bool found2 = false;
	char temp2[15];
	
	idsec.open("secAthrId.txt",ios::out|ios::in|ios::binary);
	while (first2 <= last2 && !found2)
	{
		mid2 = (first2 + last2) / 2;
		idsec.seekg(mid2 * 17, ios::beg);
		idsec.read((char*)&temp2, sizeof(temp2));

		if (strcmp(temp2, aid) == 0)
			found2 = true;
		else if (strcmp(temp2, aid) == 1)
			last2 = mid2 - 1;
		else
			first2 = mid2 + 1;
	}
	//offset of its primary index
	short of;
	idsec.seekg((mid2*17)+15,ios::beg);
	idsec.read((char*)&of,sizeof(of));
	idsec.close();
	if(found2)
	{
		//short of;
		idsec2.open("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
		idsec2.seekg((of*17)+15,ios::beg);
		idsec2.read((char*)&of,sizeof(of));
		short n= '*';
		if(of==-1)//the only one that has this secondary key then delete it
		{
			idsec2.seekg(-2,ios::cur);
			idsec2.write((char*)&n,sizeof(n));
            shiftd(aid,mid2);
		}
		else
		{
			//idsec2.seekg((of*17)+15,ios::beg);
            idsec2.seekg(-2,ios::cur);
			idsec2.write((char*)&n,sizeof(n));
			idsec.open("secAthrId.txt",ios::out|ios::in|ios::binary);
            idsec.seekg((mid2*17)+15,ios::beg);
			idsec.write((char*)&of,sizeof(of));
		}


	}

}
void shiftd(char aid[15],short mid)
{
	fstream idsec;
	streampos end,beg;
	idsec.seekg(0,ios::beg);
	beg=idsec.tellg();
	idsec.seekg(0,ios::end);
	end=idsec.tellg();
	idsec.close();
	
	CntIdSec=(end-beg)/17;
	char tid[15];
			short tmpof;
			//idsec.seekg(((CntIdSec)*17),ios::beg);
			idsec.seekg(((mid)*17),ios::beg);
			idsec.read(tid,sizeof(tid));
			idsec.read((char*)&tmpof,sizeof(tmpof));
			cout<<"from mid file"<<tid<<" =>"<<tmpof<<endl;
		/*//idsec.seekg(-17,ios::end);*/

			int j=(mid)+1;
		while(j<CntIdSec)
		{
			cout<<"in loop"<<endl;
			//char tid[15];
			//short tmpof;
			if(idsec.tellg()==ios::end)
			{
				break;
			}
			idsec.read(tid,sizeof(tid));
			idsec.read((char*)&tmpof,sizeof(tmpof));
			cout<<"from file"<<tid<<" =>"<<tmpof<<endl;
			idsec.seekp(-34,ios::cur);
			idsec.write(tid,15);
			idsec.write((char*)&tmpof,sizeof(tmpof));
			idsec.seekg(17,ios::cur);
			
			j++;
		}
        idsec.close();


}
void  deleteSecId(char aid[15])//delete Author from the secodary index of book use it after deleting an author
{
	fstream idsec;
	fstream idsec2;
	idsec.open("secAthrId.txt",ios::out|ios::in|ios::binary);
	idsec2.open("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
	streampos end,begin,lend,lbeg;
	 if (idsec.bad())
    {
            cout << "failed to open sec1" << endl;
            return ;
      }
	idsec.seekg(0,ios::beg);
	begin=idsec.tellg();
	idsec.seekg(0,ios::end);
	end=idsec.tellg();
	idsec.close();
	idsec2.seekg(0,ios::beg);
	lbeg=idsec2.tellg();
	idsec2.seekg(0,ios::end);
	lend=idsec2.tellg();
	idsec2.close();
	CntIdSec=(end-begin)/17;

	CntIdLL=(lend-lbeg)/17;
	cout<<"file count = "<<CntIdSec<<endl;
	cout<<"file count LL = "<<CntIdLL<<endl;
	short first = 0;
	short last = CntIdSec - 1;
	short mid;
	bool found = false;
	char temp[15];
	
	idsec.open("secAthrId.txt",ios::out|ios::in|ios::binary);
	while (first <= last && !found)
	{
		mid = (first + last) / 2;
		idsec.seekg(mid * 17, ios::beg);
		idsec.read((char*)&temp, sizeof(temp));

		if (strcmp(temp, aid) == 0)
			found = true;
		else if (strcmp(temp, aid) == 1)
			last = mid - 1;
		else
			first = mid + 1;
	}
	//offset of its primary index
	short of;
	idsec.seekg((mid*17)+15,ios::beg);
	idsec.read((char*)&of,sizeof(of));
	idsec.close();
	if(!found)
	{
		cout<<"not found"<<endl;
		return;
	}
	else//found
	{  
		 //1)delete secondary key by shifting 
        
	idsec.open("secAthrId.txt",ios::out|ios::in|ios::binary);
	idsec.seekg(0,ios::beg);
	begin=idsec.tellg();
	idsec.seekg(0,ios::end);
	end=idsec.tellg();
	idsec.close();
	CntIdSec=(end-begin)/17;
	idsec.open("secAthrId.txt",ios::out|ios::in|ios::binary);
    vector<pair<char[15],short> >shift;
	
        if(CntIdSec==1)
		{
			cout<<"cn"<<CntIdSec<<endl;
			idsec.clear();
		}
		else
		{
			char tid[15];
			short tmpof;
			//idsec.seekg(((CntIdSec)*17),ios::beg);
			idsec.seekg(((mid)*17),ios::beg);
			idsec.read(tid,sizeof(tid));
			idsec.read((char*)&tmpof,sizeof(tmpof));
			cout<<"from mid file"<<tid<<" =>"<<tmpof<<endl;
		/*//idsec.seekg(-17,ios::end);*/

	/*		int j=(mid)+1;
		while(j<CntIdSec)
		{
			cout<<"in loop"<<endl;
			//char tid[15];
			//short tmpof;
			if(idsec.tellg()==ios::end)
			{
				break;
			}
			idsec.read(tid,sizeof(tid));
			idsec.read((char*)&tmpof,sizeof(tmpof));
			cout<<"from file"<<tid<<" =>"<<tmpof<<endl;
			idsec.seekp(-34,ios::cur);
			idsec.write(tid,15);
			idsec.write((char*)&tmpof,sizeof(tmpof));
			idsec.seekg(17,ios::cur);
			
			pair<char[15],short> ptmp;
			memcpy(ptmp.first,tid,15);
			ptmp.second=tmpof;
            shift.push_back(ptmp);
			j++;
		}
        idsec.close();
		for(int i=0;i<shift.size();i++)
		{
			cout<<shift[i].first<<" "<<
			shift[i].second<<endl;
		}
		}
	/*	idsec.open("secAthrId.txt",ios::out|ios::in|ios::binary);
		idsec.seekg((mid)*17,ios::beg);
		for(int i=0;i<shift.size();i++)
		{
			idsec.read(shift[i].first,15);
			idsec.read((char*)&(shift[i].second),sizeof(shift[i].second));
		}
		idsec.close();*/
		/*while(i<CntIdSec-1)
		{
			cout<<"in loop"<<endl;
			char tid[15];
			short tmpof;
			idsec.read(tid,15);
			idsec.read((char*)&tmpof,sizeof(tmpof));
			idsec.seekg(-34,ios::cur);
			idsec.write(tid,15);
			idsec.write((char*)&tmpof,sizeof(tmpof));
			idsec.seekg(17,ios::cur);
			i++;
		}
		idsec.seekp(-17,ios::end);
	    idsec.write("",0);*/
		//idsec.truncate(idsec.tellg());
		//}
		
	//	idsec.close();*/
	//linked list part
	
        idsec2.open("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
		idsec2.seekg((of*17)+15,ios::beg);
		short oftmp2;
		short  n=-2;
		idsec2.read((char*)&oftmp2,sizeof(oftmp2));
		if(oftmp2==-1)//the only one that has this secondary key
		{
			idsec2.seekg(-2,ios::cur);
			idsec2.write((char*)&n,sizeof(n));
		}
		else
		{
			while(oftmp2!=-1)
			{

				idsec2.seekg(-2,ios::cur);
			    idsec2.write((char*)&n,sizeof(n));
				idsec2.seekg((oftmp2*17)+15,ios::beg);
				idsec2.read((char*)&oftmp2,sizeof(oftmp2));
			}
		}

        idsec2.close();

	}
	  
	  
   idsec.open("secAthrId.txt",ios::out|ios::in|ios::binary);
	//idsec2.open("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
	char idtemp[15];
	short idoffset;


	idsec.read(idtemp,15);
	idsec.read((char*)&idoffset,sizeof(idoffset));
	while ((idsec.good()))
	{
		cout<<"aid in secondary ="<<idtemp<<" "<<"off ="<<idoffset<<endl;
		idsec.read(idtemp,15);
	    idsec.read((char*)&idoffset,sizeof(idoffset));
	}
	idsec.close();
	idsec2.open("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
	idsec2.read(idtemp,15);
	idsec2.read((char*)&idoffset,sizeof(idoffset)); 
    int i=0;
    while (idsec2.good())
	{
		cout<<"isbn in secondary ="<<idtemp<<" "<<"next ="<<idoffset<<endl;
		idsec2.read(idtemp,15);
	    idsec2.read((char*)&idoffset,sizeof(idoffset));
		i++;
	}
    idsec2.close();

}

}
void insertsortedAuthor(char id[], short offset,string fileName){
    fstream prim(fileName, ios::out | ios::in|ios::binary);

	int x = 0;
	//id -> int
	for (int i = 0; id[i] != '\0'; i++)
	{
		x *= 10;
		x += (id[i] - '0');
	}
	int tmp = 0;
	short of = 0;
	bool foundPlaceBetween = false;
	if (CntIDauthor == 0)//if index file is empty
	{
		prim.write((char*)&x, sizeof(int));
		prim.write((char*)&offset, sizeof(short));

		CntIDauthor++;
		return;//write and exit
	}
	prim.read((char*)&tmp, sizeof(tmp));
	while (prim.good())
	{//until end of file or until you find an index greater than the index you want to insert
		if (tmp > x)
		{
			foundPlaceBetween = true;
			prim.seekg(-4, ios::cur);
			of = prim.tellg();
			//cout<<"of is "<<of;
			break;
		}

		prim.seekg(2, ios::cur);//skip byte offset
		prim.read((char*)&tmp, sizeof(tmp));//read index

	}
	prim.close();
	prim.open(fileName, ios::out | ios::in|ios::binary);

	if (!foundPlaceBetween){ ///in the last position

		prim.seekg(CntIDauthor * 6, ios::beg);
		prim.write((char*)&x, sizeof(int));
		prim.write((char*)&offset, sizeof(short));
		CntIDauthor++;
	}
	else{
		prim.seekg((CntIDauthor - 1) * 6, ios::beg);
		int numend;
		short ofend;
		//reading the last index and saving it
		prim.read((char*)& numend, sizeof(numend));
		prim.read((char*)& ofend, sizeof(ofend));

		prim.seekg(of, ios::beg);



		while (prim.good())//shifting
		{
			int tmpnum; short tmpof;
			int tmpnum1; short tmpof1;
			prim.read((char*)& tmpnum, sizeof(tmpnum));
			prim.read((char*)& tmpof, sizeof(tmpof));
			prim.read((char*)& tmpnum1, sizeof(tmpnum1));
			prim.read((char*)& tmpof1, sizeof(tmpof1));
			prim.seekp(-6, ios::cur);
			prim.write((char*)& tmpnum, sizeof(tmpnum));
			prim.write((char*)& tmpof, sizeof(tmpof));

		}
		prim.close();
		prim.open(fileName, ios::out | ios::in  |ios::binary);
		prim.seekp(0, ios::end);
        // seek to end of file and write last index
		prim.write((char*)& numend, sizeof(numend));
		prim.write((char*)& ofend, sizeof(ofend));
		prim.seekp(of, ios::beg);
		// write index
		prim.write((char*)& x, sizeof(x));
		prim.write((char*)& offset, sizeof(of));
		CntIDauthor++;

	}
	prim.close();
	prim.open(fileName, ios::out | ios::in|ios::binary);
	int tt;
	short rr;
	prim.read((char*)&tt, sizeof(tt));
	prim.read((char*)&rr, sizeof(rr));
	while (prim.good())
	{

		cout << "ID In primary = " << tt << " Off = " << rr << endl;
		prim.read((char*)&tt, sizeof(tt));
		prim.read((char*)&rr, sizeof(rr));

	}


	prim.close();
}
void insertsortedBooks(char isbn[15], short offset,string fileName){
    fstream prim(fileName, ios::out | ios::in|ios::binary);

	char tmp[15] ;
	short of = 0;
	bool foundPlaceBetween = false;
	if (CntIDbook == 0)//if index file is empty
	{
	    char tt[15];
        short rr;
        //cout<<(char*)&isbn;
		prim.write(isbn, 15);
		prim.write((char*)&offset, sizeof(short));
		prim.close();
	    prim.open(fileName, ios::out | ios::in|ios::binary);
        prim.read(tt, 15);
        prim.read((char*)&rr, sizeof(rr));
		CntIDbook++;
		cout << "ID In primary = " << tt << " Off = " << rr << endl;
		return;//write and exit
	}
	prim.read((char*)&tmp, 15);
	while (prim.good())
	{//until end of file or until you find an index greater than the index you want to insert
		if (tmp > isbn)
		{
			foundPlaceBetween = true;
			prim.seekg(-15, ios::cur);
			of = prim.tellg();
			break;
		}

		prim.seekg(2, ios::cur);//skip byte offset
		prim.read((char*)&tmp, 15);//read index

	}
	prim.close();
	prim.open(fileName, ios::out | ios::in|ios::binary);

	if (!foundPlaceBetween){ ///in the last position

		prim.seekg(CntIDbook * 17, ios::beg);
		prim.write(isbn, 15);
		prim.write((char*)&offset, sizeof(short));
		CntIDbook++;
	}
	else{
		//prim.seekg((CntIDbook - 1) * 17, ios::beg);
        prim.seekg(- 17, ios::end);
		char ISBNend[15];
		short ofend;
		//reading the last index and saving it
		prim.read( ISBNend, 15);
		prim.read((char*)& ofend, sizeof(ofend));

		prim.seekg(of, ios::beg);



		while (prim.good())//shifting
		{
			char tmpnum[15]; short tmpof;
			char tmpnum1[15]; short tmpof1;
			prim.read( tmpnum, 15);
			prim.read((char*)& tmpof, sizeof(tmpof));
			prim.read( tmpnum1, 15);
			prim.read((char*)& tmpof1, sizeof(tmpof1));
			prim.seekp(-17, ios::cur);
			prim.write( tmpnum, 15);
			prim.write((char*)& tmpof, sizeof(tmpof));

		}
		prim.close();
		prim.open(fileName, ios::out | ios::in |ios::binary);
		prim.seekp(0, ios::end);
        // seek to end of file and write last index
		prim.write( ISBNend, 15);
		prim.write((char*)& ofend, sizeof(ofend));
		prim.seekp(of, ios::beg);
		// write index
		prim.write( isbn, 15);
		prim.write((char*)& offset, sizeof(offset));
		CntIDbook++;

	}
	prim.close();
	prim.open(fileName, ios::out | ios::in|ios::binary);
	char tt[15];
	short rr;
	prim.read(tt, 15);
	prim.read((char*)&rr, sizeof(rr));
	while (prim.good())
	{

		cout << "ID In primary = " << tt << " Off = " << rr << endl;
		prim.read(tt, 15);
		prim.read((char*)&rr, sizeof(rr));

	}


	prim.close();
}
short BinarySearchIdauthor(int ID1) // binary search in the primary index
{
	fstream prim("PIauthors.txt", ios::in | ios::binary | ios::out);
	short first = 0;
	short last = CntIDauthor - 1;
	short mid;
	bool found = false;
	int temp;
	while (first <= last && !found)
	{
		mid = (first + last) / 2;
		prim.seekg(mid * 6, ios::beg);///??
		prim.read((char*)&temp, sizeof(temp));

		if (temp == ID1)
			found = true;
		else if (temp >  ID1)
			last = mid - 1;
		else
			first = mid + 1;
	}
	if (found)
	{
		short of;
		prim.seekg((mid * 6) + 4, ios::beg);
		prim.read((char*)&of, sizeof(of));
		prim.close();
		return of;
	}
	else
	{
	    short notFound=-1;
		prim.close();
		return notFound;

	}

}
short BinarySearchIdbook(char isbn[]) // binary search in the primary index
{
	fstream prim("PIbooks.txt", ios::in | ios::binary | ios::out);
	short first = 0;
	short last = CntIDbook - 1;
	//cout<<"CntIDbook: "<<CntIDbook;
	short mid;
	bool found = false;
	//string temp,ISBN(isbn);
	char temp[15];
	string ISBN(isbn);
	while (first <= last && !found)
	{

		mid = (first + last) / 2; // cout<<"mid"<<mid;
		prim.seekg(mid * 17, ios::beg);///??
		prim.read(temp, 15);
        //cout<<temp;
		if (temp == ISBN)
			found = true;
		else if (temp >  ISBN)
			last = mid - 1;
		else
			first = mid + 1;
	}
	if (found)
	{
		short of;
		prim.seekg((mid * 17) + 15, ios::beg);
		prim.read((char*)&of, sizeof(of));
		prim.close();
		return of;
	}
	else
	{
	    short notFound=-1;
		prim.close();
		return notFound;

	}
}
void Deleteprimary(int ID1) // delete from primary index
{

	fstream prim("PIauthors.txt", ios::in | ios::binary | ios::out);
	short first = 0;
	short last = CntIDauthor - 1;
	short mid;
	bool found = false;
	int temp;
	while (first <= last && !found)
	{
		mid = (first + last) / 2;
		prim.seekg(mid * 6, ios::beg);
		prim.read((char*)&temp, sizeof(temp));

		if (temp == ID1)
			found = true;
		else if (temp >  ID1)
			last = mid - 1;
		else
			first = mid + 1;
	}
	if (found)
	{
		prim.seekg((mid + 1) * 6, ios::beg);

		while (prim.good()) /// start to shift
		{
			int tmpnum; short tmpof;
			prim.read((char*)& tmpnum, sizeof(tmpnum));
			prim.read((char*)& tmpof, sizeof(tmpof));

			prim.seekg(-12, ios::cur);
			prim.write((char*)& tmpnum, sizeof(tmpnum));
			prim.write((char*)& tmpof, sizeof(tmpof));
			prim.seekg(6, ios::cur);

		}
		prim.close();
		prim.open("PIauthors.txt", ios::out | ios::in | ios::binary);

		CntIDauthor--;

	}
	prim.close();
}

void printAuthor(int id){
     short off=BinarySearchIdauthor(id);
    if(off<0){
        cout<<"no such ID\n";

    }
    else{
    fstream file("test.txt", ios::out |ios::in | ios::binary | ios::app);
    string l="";
    file.seekg(off,ios::beg);
    char c;
    file.read((char*)&c,sizeof(c));
    while(c!='|'){
        l+=c;
        file.get(c);
    }
   int length=stoi(l);
    string buf;
    file.getline((char*)&buf,length);
    string record(buf);
    //cout<<record;
    int pos=record.find('|');
    string  ID,name,address;
    ID=record.substr(0,pos);
    cout<<"AUTHOR'S ID : \t"<<ID<<endl;
    record.erase(0,ID.length()+1);
    pos=record.find('|');
    name=record.substr(0,pos);
    cout<<"AUTHOR'S NAME : \t"<<name<<endl;
    record.erase(0,name.length()+1);
    pos=record.find('|');
    address=record.substr(0,pos);
    cout<<"AUTHOR'S ADDRESS : \t"<<address<<endl;
    file.close();
    }


}
void printbook(char isbn[] ){
   short off=BinarySearchIdbook(isbn);
    //cout<<off;
    if(off<0){
        cout<<"no such ISBN\n";

    }
    else{
    fstream file("books.txt", ios::out |ios::in | ios::binary | ios::app);
    string l="";
    file.seekg(off,ios::beg);
    char c;
    file.read((char*)&c,sizeof(c));
    while(c!='|'){
        l+=c;
        file.get(c);
    }
    int length=stoi(l);
    string buf;
    file.getline((char*)&buf,length);
    string record(buf);
    //cout<<record;
    int pos=record.find('|');
    string  ID,name,address;
    ID=record.substr(0,pos);
    cout<<"ISBN : \t"<<ID<<endl;
    record.erase(0,ID.length()+1);
    pos=record.find('|');
    name=record.substr(0,pos);
    cout<<"TITLE : \t"<<name<<endl;
    record.erase(0,name.length()+1);
    pos=record.find('|');
    address=record.substr(0,pos);
    cout<<"AUTHOR'S ID : \t"<<address<<endl;
     file.close();
    }

}


void Delete(char id[],string file_name)
{
    short M = -1;
    fstream file;
    file.open(file_name, ios::in | ios::out | ios::binary);
    int x = 0;
    //id -> int
    for (int i = 0; id[i] != '\0'; i++)
    {
        x *= 10;
        x += (id[i] - '0');
    }
    short  Offset = BinarySearchIdauthor(x), Size,header; // read offset
    file.seekg(Offset , ios::beg);
    file.seekg(Offset - 2, ios::beg);
    file.read((char*)&Size, sizeof(Size)); 
    if (Offset == -1)
        cout << "ID doesnt exist. " << endl;
    else
    {//id found
        file.seekg(0, ios::beg);
        file.read((char*)&header, sizeof(header)); //read the header

        if (header == -1) //the avail list is empty
        {
            file.seekg(0, ios::beg);
            file.write((char*)&Offset, sizeof(Offset));
            file.seekg(Offset, ios::beg);
            file.write("*", 1);
            file.write((char*)&M, sizeof(M));
            file.close();
        }
        else //the avail list not empty
        { 


            file.seekg(0, ios::beg);
            file.read((char*)&header, sizeof(header)); 
            short HeaderSize;
            file.seekg(header - 2, ios::beg);
            file.read((char*)&HeaderSize, sizeof(HeaderSize)); // read header size
            if (Size >= HeaderSize) // if deleted size >= header size
            {
                file.seekg(0, ios::beg);
                file.write((char*)&Offset, sizeof(Offset)); // write offset in the head
                file.seekg(Offset, ios::beg);
                file.write("*", 1);
                file.write((char*)&header, sizeof(header)); // write header offset in deleted item
                file.close();

            }
            else // deleted size  < header size
            {

                file.seekg(0, ios::beg);
                short OffsetPre, SizeNext, TempOffset;
                OffsetPre = header;
                file.seekg(header + 1, ios::beg);
                while (true)
                {


                    file.read((char*)&TempOffset, sizeof(TempOffset));
                    if (TempOffset == -1) // if reached the end of list
                    {
                        file.seekg(Offset, ios::beg);
                        file.write("*", 1);
                        file.write((char*)&M, sizeof(M));
                        file.seekg(OffsetPre + 1, ios::beg);
                        file.write((char*)&Offset, sizeof(Offset));
                        file.close();
                        break;
                    }
                    else
                    {
                        file.seekg(TempOffset - 2, ios::beg);
                        file.read((char*)&SizeNext, sizeof(SizeNext));
                        if (Size >= SizeNext) // if  temp size <= deleted size
                        {
                            file.seekg(Offset, ios::beg);
                            file.write("*", 1);
                            file.write((char*)&TempOffset, sizeof(TempOffset));
                            file.seekg(OffsetPre + 1, ios::beg);
                            file.write((char*)&Offset, sizeof(Offset));
                            file.close();
                            break;

                        }
                        else // move to the next record
                        {
                            OffsetPre = TempOffset;
                            file.seekg(OffsetPre + 1, ios::beg);
                        }
                    }



                }

                file.close();
            }

        }
    }



}

class Author {
public:
    char Author_ID[15]; //primary key
    char Author_Name[30];
    char Address[30];

    void saveAthr()
    {
        fstream file("datafile.txt", ios::out | ios::binary | ios::in);
        short record_size,id_size,name_size,address_size,header;
        id_size= strlen(Author_ID);
        name_size= strlen(Author_Name);
        address_size= strlen(Address);
        record_size=id_size+name_size+address_size+4;
        file.seekg(0, ios::beg);
        file.read((char*)&header, sizeof(header)); //read header
        int x = 0;
        //id -> int
        for (int i = 0; Author_ID[i] != '\0'; i++)
        {
            x *= 10;
            x += (Author_ID[i] - '0');
        }
        if (BinarySearchIdauthor(x)!=-1){//If the record to be added doesnt exit
            if (header==-1){//the avail list is empty
                file.seekp(0,ios::end);
                file.write((char*)&record_size, sizeof(record_size));
                insertsortedAuthor(Author_ID,file.tellp(),"PIauthors.txt");
                file.write("|",1);
                file.write((char*)&id_size, sizeof(id_size) );
                file.write("|",1);
                file.write((char*)&name_size, sizeof(name_size) );
                file.write("|",1);
                file.write((char*)&address_size, sizeof(address_size) );
                file.write("|",1);
                file.close();

            }
            else{//the avail list not empty
                short next_offset, deleted_size;

                file.seekg(header + 1, ios::beg);//to skip asterisk
                file.read((char*)&next_offset, sizeof(next_offset)); // read the next offset after the header
                file.seekg(header - 2, ios::beg);//to read the size of deleted record
                file.read((char*)&deleted_size, sizeof(deleted_size));
                short DifferSize = deleted_size - record_size;
                if (DifferSize<0){//record_size>deleted_size
                    file.seekp(0,ios::end);
                    file.write((char*)&record_size, sizeof(record_size));
                    insertsortedAuthor(Author_ID,file.tellp(),"PIauthors.txt");
                    file.write("|",1);
                    file.write((char*)&id_size, sizeof(id_size) );
                    file.write("|",1);
                    file.write((char*)&name_size, sizeof(name_size) );
                    file.write("|",1);
                    file.write((char*)&address_size, sizeof(address_size) );
                    file.write("|",1);
                    file.close();
                }
                else{
                    if (DifferSize - 2 <= 5){ /// if the difference  <= 5

                        file.seekp(0, ios::beg);
                        file.write((char*)&next_offset, sizeof(next_offset));

                        file.seekp(header - 2, ios::beg);
                        insertsortedAuthor(Author_ID, header,"PIauthors.txt");


                        record_size += DifferSize;
                        file.write((char*)&record_size, sizeof(record_size));
                        file.write("|",1);
                        file.write((char*)&id_size, sizeof(id_size) );
                        file.write("|",1);
                        file.write((char*)&name_size, sizeof(name_size) );
                        file.write("|",1);
                        file.write((char*)&address_size, sizeof(address_size) );
                        file.write("|",1);
                        file.close();


                    }
                    else // if the difference > 5
                    {

                        file.seekp(0, ios::beg);
                        file.write((char*)&next_offset, sizeof(next_offset));
                        file.seekp(header - 2, ios::beg);
                        insertsortedAuthor(Author_ID, header,"PIauthors.txt");


                        file.write((char*)&record_size, sizeof(record_size));
                        file.write("|",1);
                        file.write((char*)&id_size, sizeof(id_size) );
                        file.write("|",1);
                        file.write((char*)&name_size, sizeof(name_size) );
                        file.write("|",1);
                        file.write((char*)&address_size, sizeof(address_size) );
                        file.write("|",1);
                        file.seekg(-(name_size=id_size+address_size+4), ios::cur); /// seek to the beg of the record
                        file.seekg(record_size, ios::cur); /// seek after the new record size

                        short ID_Fake = 1; /// make a fake record with the rest of the original record
                        DifferSize -= 2;
                        file.write((char*)&DifferSize, sizeof(DifferSize));

                        file.write((char*)&ID_Fake, sizeof(ID_Fake));
                        file.write("#", 1);
                        file.write((char*)&ID_Fake, sizeof(ID_Fake));
                        file.write("1", 1);

                        file.close();
                        Delete("1",""); /// then delete it to push in the avail list
                    }
                }


            }
        }
    }
};
class Book {
public:
    char ISBN[15]; //primary key
    char book_Title[30];
    char author_ID[15]; // secondary key
    void saveBook(fstream& file, string name="books.txt")
    {
        file.open(name, ios::out | ios::binary | ios::app);
        if (file.fail())
        {
            cout << "failed to open" << endl;
            return;
        }
        int size = string(ISBN).size() + string(book_Title).size() + string(author_ID).size() + 4;
        file.seekp(0,ios_base::end);
        short end1 = file.tellp();
        file<<size << '|' <<ISBN << '|' << book_Title << '|' <<author_ID << '|';
        file.close();
        insertsortedBooks(ISBN,end1,"PIbooks.txt");
        insertsortedAidSec(author_ID,ISBN);
    }
};


void selectQuery()
{
	string what,table,condition;
	cout<<"Select";
	getline(cin,what);
	cout<<"From";
	cin>>table;
	cout<<"where";
	getline(cin,condition);
	//cin.ignore;
	if(table=="Authors")
	{
		if(what=="all")
		{
			
		}
	}

}
void mainMenu()
{
    fstream file;
	while (true)
	{
		int choice;
	    cout<<"welcome pls enter you choice\n enter 1 Add New Author\n enter 2 Add New Book enter 3 Update Author Name"<<endl;
	    cout<<"enter 4 to Update Book Title (ISBN)"<<endl<<"enter 5 to  Delete Book (ISBN)"<<endl;
	    cout<<"enter 6 to  Delete Author (Author ID) "<<endl<<"enter 7 to Print Author (Author ID) "<<endl<<"enter 8 to Print Book (ISBN)"<<endl;
	    cout<<"enter 9 to write a query"<<endl<<"or 10 to exist"<<endl;
		cin>>choice;
		if(choice==1)
		{ 
			 Author a1;
			 cout << "pls enter name " << endl;
             cin.getline(a1.Author_Name, 30);
             cout << "pls enter id: ";
             cin.getline(a1.Author_ID, 15);
              cout << "pls enter address:";
               cin.getline(a1.Address, 30);
              a1.saveAthr();
		}
		else if(choice==2)
		{
			 Book b1;
			cout << "pls enter isbn " << endl;
           cin.getline(b1.ISBN, 15);
           cout << "pls enter title: ";
           cin.getline(b1.book_Title, 30);
         cout << "pls enter author's id :";
          cin.getline(b1.author_ID, 15);
          b1.saveBook(file);

		}
		else if(choice==3)
		{
			
		}
		else if(choice==4)
		{
			
		}
		else if(choice==5)
		{
			
		}
		else if(choice==6)
		{
			
		}
		else if(choice==7)
		{
			
		}
		else if(choice==8)
		{
			
		}
		else if(choice==9)
		{
			
		}
		else
		{
			break;
		}
	}
	
	
}
/*
int main()
{
        fstream primA("PIauthors.txt", ios::out | ios::in|ios::app|ios::binary);
        primA.close();
        fstream primB("PIbooks.txt", ios::out | ios::in|ios::app|ios::binary);
        primB.close();

    fstream info("info.txt",ios::in | ios::out |ios::binary|ios::app);
    info.read((char*)& CntIDauthor, sizeof(CntIDauthor));
    info.read((char*)& CntIDbook, sizeof(CntIDbook));
    info.close();
   // cout<<"cntid books : "<<CntIDbook<<endl;

    //cout<<"cntid : "<<CntID<<endl;

    fstream fileAuthors;
    string name = "test.txt";
    int n=5;
    Author a1;
    Book b1;
    while(n--){
        cout << "pls enter name " << endl;
        cin.getline(a1.Author_Name, 30);
        cout << "pls enter id: ";
        cin.getline(a1.Author_ID, 15);
        cout << "pls enter address:";
        cin.getline(a1.Address, 30);
        a1.saveAthr();
    }
  //  printAuthor(3);
//    while(n--){
//        cout << "pls enter isbn " << endl;
//        cin.getline(b1.ISBN, 15);
//        cout << "pls enter title: ";
//        cin.getline(b1.book_Title, 30);
//        cout << "pls enter author's id :";
//        cin.getline(b1.author_ID, 15);
//        b1.saveBook(fileAuthors);
//    }

//printbook("111111-111-66");
 info.open("info.txt",ios::in | ios::out |ios::binary|ios::app);
 info.clear();
 info.write((char*)& CntIDauthor, sizeof(CntIDauthor));
 info.write((char*)& CntIDbook, sizeof(CntIDbook));
//cout<<"cntid books : "<<CntIDbook<<endl;
info.close();
    return 0;
}*/
int main()
{
        fstream primA("PIauthors.txt", ios::out | ios::in|ios::app|ios::binary);
        primA.close();
        fstream primB("PIbooks.txt", ios::out | ios::in|ios::app|ios::binary);
        primB.close();

    fstream info("info.txt",ios::in | ios::out |ios::binary|ios::app);
    info.read((char*)& CntIDauthor, sizeof(CntIDauthor));
    info.read((char*)& CntIDbook, sizeof(CntIDbook));
    info.close();
   // cout<<"cntid books : "<<CntIDbook<<endl;

    //cout<<"cntid : "<<CntID<<endl;
    fstream idsec("secAthrId.txt",ios::out|ios::in|ios::binary|ios::app);
	fstream  idsec2("athrIDsecLL.txt",ios::out|ios::in|ios::binary|ios::app);
    idsec.close();
    idsec2.close();
    fstream fileAuthors;
    string name = "test.txt";
    int n=3;
    Author a1;
    Book b1;
   /* while(n--){
        cout << "pls enter name " << endl;
        cin.getline(a1.Author_Name, 30);
        cout << "pls enter id: ";
        cin.getline(a1.Author_ID, 15);
        cout << "pls enter address:";
        cin.getline(a1.Address, 30);
        a1.saveAthr();
    }*/
  //  printAuthor(3);
 /**/
 while(n--){
    
        cout << "pls enter isbn " << endl;
        cin.getline(b1.ISBN, 15);
        cout << "pls enter title: ";
        cin.getline(b1.book_Title, 30);
        cout << "pls enter author's id :";
        cin.getline(b1.author_ID, 15);
    //    b1.saveBook(fileAuthors);
    

		//if(n==3)
		//{
            searchBySecId(b1.author_ID);
            //printbook(b1.ISBN);
			//deleteSecId(b1.author_ID);34376253
		//}

   }   /**/
  /* fstream idsec,idsec2;
   idsec.open("secAthrId.txt",ios::out|ios::in|ios::binary);
	//idsec2.open("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
	char idtemp[15];
	short idoffset;


	idsec.read(idtemp,15);
	idsec.read((char*)&idoffset,sizeof(idoffset));
	while ((idsec.good()))
	{
		cout<<"aid in secondary ="<<idtemp<<" "<<"off ="<<idoffset<<endl;
		idsec.read(idtemp,15);
	    idsec.read((char*)&idoffset,sizeof(idoffset));
	}
	idsec.close();
	idsec2.open("athrIDsecLL.txt",ios::out|ios::in|ios::binary);
	idsec2.read(idtemp,15);
	idsec2.read((char*)&idoffset,sizeof(idoffset)); 
    int i=0;
    while (idsec2.good())
	{
		cout<<"isbn in secondary ="<<idtemp<<" "<<"next ="<<idoffset<<endl;
		idsec2.read(idtemp,15);
	    idsec2.read((char*)&idoffset,sizeof(idoffset));
		i++;
	}
    idsec2.close();*/


//printbook("111111-111-66");
 info.open("info.txt",ios::in | ios::out |ios::binary|ios::app);
 info.clear();
 info.write((char*)& CntIDauthor, sizeof(CntIDauthor));
 info.write((char*)& CntIDbook, sizeof(CntIDbook));
//cout<<"cntid books : "<<CntIDbook<<endl;
info.close();
    return 0;
}
