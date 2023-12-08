#include<bits/stdc++.h>
using namespace std;

struct Author
{
    char AuthorID[15] ; //primary key
    char AuthorName[30] ;
    char Address[30] ;
};

struct book
{
    char ISBN[15]  ;//primary key
    char BookTitle[30] ;
    char AuthorID[15] ; // secondary
};



short CntIDauthor = 0 , CntIDbook = 0, CntIdSec,CntIdLL;

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

    int offset = BinarySearchIdauthor(authorID);
    if (offset == -1) {
        cout << "**Author with ID " << authorID << " is not found.**" << endl;
        return;
    }

    int oldNameSize = getNameSizeAuthor(authorsFile, offset + strlen(authorID) + 1);
    std::cout << "Old Name Size: " << oldNameSize << std::endl;
    authorsFile.seekp(offset + strlen(authorID) + 1 + sizeof(short), ios::beg);


    if (strlen(newName) < oldNameSize) {
        string paddedName = newName + std::string(oldNameSize - strlen(newName), ' ');
        authorsFile.write(paddedName.c_str(), oldNameSize);
    } else if (strlen(newName) == oldNameSize) {
        authorsFile.write(newName, oldNameSize);
    } else {
        std::cout << "New Author Name is bigger than the old one, please enter smaller than or equal to the old one!" << std::endl;
    }

    authorsFile.close();
    authorsPrimaryIndex.close();
}







int main()
{


}