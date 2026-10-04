//IN THIS PACKAGE WE HAVE TRIED TO GIVE A COMPACT REPRESENTATION FOR AN IMAGE USING QUAD TREE AND PERFORM OPERATIONS VIEW , CUT AND ZOOM ON IT
//
// A quadtree splits the image into 4 quadrants, recursively. If every pixel in a quadrant has the
// same value, the quadrant is stored as ONE leaf instead of being split further. That merge is what
// makes it compact: large flat areas (background, sky) cost one node instead of thousands.
#include<iostream>
#include<fstream>
#include<stdlib.h>
using namespace std;
const int Size=256;
int picture[Size][Size];
int Array[Size][Size];
int Nodes=0, Leaves=0;
struct QuadTree
{
    int pixel;                                              //-1 FOR AN INTERNAL NODE, ELSE THE COLOUR OF THE WHOLE QUADRANT
    QuadTree *topright,*topleft,*bottomright,*bottomleft;
};

QuadTree *root=NULL;

QuadTree *allocate(int pixel)                               //THIS FUNCTION ALLOCATES MEMORY FOR A NODE DYNAMICALLY
{
    QuadTree *temp=new QuadTree;
    temp->pixel=pixel;
    temp->topright=temp->topleft=temp->bottomright=temp->bottomleft=NULL;
    Nodes++;
    if(pixel!=-1) Leaves++;
    return temp;
}

void ReadImage()                                            //THIS FUNCTION READS THE PIXEL VALUE INTO AN ARRAY FROM A FILE
{
    ifstream fin("image.txt",ios::in);
    if(!fin)
    {
        cout<<"\nError: image.txt not found. Run: python3 image_to_pixel.py\n";
        exit(1);
    }
    for(int i=0;i<Size;i++)
    {
        for(int j=0;j<Size;j++)
        {
            if(!(fin>>picture[i][j]))
            {
                cout<<"\nError: image.txt must hold "<<Size*Size<<" grey values (a "<<Size<<"x"<<Size<<" image)\n";
                exit(1);
            }
        }
    }
}

QuadTree *Insert(int x1,int x2,int y1,int y2)               //THIS FUNCTION GIVES THE QUADTREE FOR THE ARRAY (x = ROW, y = COLUMN)
{
    if(x1==x2 && y1==y2)
        return allocate(picture[x1][y1]);

    int Mid1=(x1+x2)/2;
    int Mid2=(y1+y2)/2;
    QuadTree *tr = Insert(x1,Mid1,Mid2+1,y2);
    QuadTree *tl = Insert(x1,Mid1,y1,Mid2);
    QuadTree *bl = Insert(Mid1+1,x2,y1,Mid2);
    QuadTree *br = Insert(Mid1+1,x2,Mid2+1,y2);

    // ALL FOUR CHILDREN ARE LEAVES OF THE SAME COLOUR -> THE QUADRANT IS UNIFORM, KEEP ONE LEAF
    if(tr->pixel!=-1 && tr->pixel==tl->pixel && tr->pixel==bl->pixel && tr->pixel==br->pixel)
    {
        int pixel=tr->pixel;
        delete tr; delete tl; delete bl; delete br;
        Nodes-=4; Leaves-=4;
        return allocate(pixel);
    }

    QuadTree *temp=allocate(-1);
    temp->topright=tr;
    temp->topleft=tl;
    temp->bottomleft=bl;
    temp->bottomright=br;
    return temp;
}

void Retrieval(QuadTree *ptr,int x1,int x2,int y1,int y2)   //THIS FUNCTION RETRIVES THE ARRAY BACK FROM QUADTREE INTO THE GIVEN REGION
{
    if(ptr==NULL)
        return;
    if(ptr->pixel==-1)
    {
        int Mid1=(x1+x2)/2;
        int Mid2=(y1+y2)/2;
        Retrieval(ptr->topright,x1,Mid1,Mid2+1,y2);
        Retrieval(ptr->topleft,x1,Mid1,y1,Mid2);
        Retrieval(ptr->bottomleft,Mid1+1,x2,y1,Mid2);
        Retrieval(ptr->bottomright,Mid1+1,x2,Mid2+1,y2);
    }
    else
    {
        // A LEAF COVERS ITS WHOLE REGION (A MERGED QUADRANT, OR A PIXEL STRETCHED BY ZOOM)
        for(int i=x1;i<=x2;i++)
            for(int j=y1;j<=y2;j++)
                Array[i][j]=ptr->pixel;
    }
}

int AskQuadrant(const char *action)
{
    int choice;
    cout<<endl<<"\t\tPortion to "<<action<<": 1.top right  2.top left  3.bottom left  4.bottom right";
    cout<<endl<<"\t\tEnter your choice:";
    while(!(cin>>choice) || choice<1 || choice>4)
    {
        if(cin.eof()) exit(1);
        cin.clear(); cin.ignore(1000,'\n');
        cout<<"\t\tPick 1-4:";
    }
    return choice;
}

// MOVES ptr AND ITS REGION INTO THE CHOSEN QUADRANT
void Descend(QuadTree *&ptr,int choice,int &x1,int &x2,int &y1,int &y2)
{
    int Mid1=(x1+x2)/2, Mid2=(y1+y2)/2;
    switch(choice)
    {
        case 1: x2=Mid1;   y1=Mid2+1; break;
        case 2: x2=Mid1;   y2=Mid2;   break;
        case 3: x1=Mid1+1; y2=Mid2;   break;
        case 4: x1=Mid1+1; y1=Mid2+1; break;
    }
    // A MERGED LEAF HAS NO CHILDREN: EVERY SUB-QUADRANT IS THE SAME LEAF
    if(ptr->pixel!=-1)
        return;
    switch(choice)
    {
        case 1: ptr=ptr->topright;    break;
        case 2: ptr=ptr->topleft;     break;
        case 3: ptr=ptr->bottomleft;  break;
        case 4: ptr=ptr->bottomright; break;
    }
}

bool AskMore(const char *action)
{
    char ch;
    cout<<"\n Do you wish to "<<action<<" further (y/n) ? ";
    return (cin>>ch) && ch=='y';
}

void Cut(QuadTree *ptr)                                     //THIS FUNCTION CUTS THE PORTION SPECIFIED BY THE USER, THE REST TURNS WHITE
{
    int x1=0,x2=Size-1,y1=0,y2=Size-1;
    do
        Descend(ptr,AskQuadrant("cut"),x1,x2,y1,y2);
    while(x1<x2 && AskMore("cut"));
    for(int i=0;i<Size;i++)
        for(int j=0;j<Size;j++)
            Array[i][j]=255;
    Retrieval(ptr,x1,x2,y1,y2);
}

void Zoom(QuadTree *ptr)                                    //THIS FUNCTION ZOOMS THE PORTION SPECIFIED BY THE USER
{
    int x1=0,x2=Size-1,y1=0,y2=Size-1;
    do
        Descend(ptr,AskQuadrant("zoom"),x1,x2,y1,y2);
    while(x1<x2 && AskMore("zoom"));
    // DRAW THE SUBTREE INTO THE FULL IMAGE: EVERY LEAF GROWS BY THE SAME FACTOR (NEAREST-NEIGHBOUR ZOOM)
    Retrieval(ptr,0,Size-1,0,Size-1);
}

void WriteFile()                                            //THIS FUNCTION WRITES THE PIXEL VALUE INTO A NEW FILE, SHOWN BY pixel_to_image.py
{
    ofstream fout("op_pixel.txt",ios::out);
    for(int i=0;i<Size;i++)
        for(int j=0;j<Size;j++)
            fout<<Array[i][j]<<'\n';
}

int main()
{
    int choice;
    ReadImage();
    root=Insert(0,Size-1,0,Size-1);
    cout<<"\n\t\tQuadtree built: "<<Nodes<<" nodes ("<<Leaves<<" leaves) for "<<Size*Size<<" pixels";
    cout<<"\n\t\tA tree without merging would need "<<(4*Size*Size-1)/3<<" nodes\n";
    do
    {
        cout<<endl<<"\t\t**********************************";
        cout<<endl<<"\t\t**   1.View image               **";
        cout<<endl<<"\t\t**   2.Cut one portion          **";
        cout<<endl<<"\t\t**   3.Zoom In                  **";
        cout<<endl<<"\t\t**********************************";
        cout<<endl<<"\t\tEnter your choice:";
        if(!(cin>>choice))
            break;
        switch(choice)
        {
            case 1: Retrieval(root,0,Size-1,0,Size-1); break;
            case 2: Cut(root); break;
            case 3: Zoom(root); break;
            default: cout<<"\t\tPick 1-3\n"; continue;
        }
        WriteFile();
        cout<<"\t\tPixel valued file has been produced (op_pixel.txt). Run: python3 pixel_to_image.py";
    }
    while(AskMore("continue"));
    cout<<endl;
    return 0;
}
