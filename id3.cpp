#include <iostream>
#include <vector>
#include <bits/stdc++.h>
#include <set>
using namespace std;
ofstream output_rules("output-Rules.txt");
//	The generate rules are written to this file found in the same directory.

vector < set<int> > attrs;
// 	A vector of sets, used to generate all the attribute values that any of the six attributes can take
//	instead of us feeding it manually we allow the system to calculate it themselves

vector < vector<int> > attrs_new;
//  A Vector of Vectors that is derived from attrs defined above. It has the same content as attrs after all the attribute
//	are generated. This implementation is due to ease in traversing vector compared to traversing a set.

vector <int> attrs_temp;
//  A Helper vector to convert attrs to attr_new.
set<int> values;
//  The set that is fed to attrs after reading in all the possible attribute values that an attribute can take.

vector< vector<int> > dataset;
//	Dataset for storing the training set to build the tree.
vector< vector<int> > ds;			//copy of dstemp
vector< vector<int> > ds1;		//new created dataset from dstemp for further use
vector< vector<int> > dstemp;		//dataset for storing temp ds for current use
vector< vector<int> > ds2;

vector< vector<int> > test_data;
//  Dataset to hold the test attributes while finally running the tree, implemented as a vector of vector

std::vector<int>::iterator it;
// 	Iterator to help with inserting elements in a vector.

vector<int> data;
// 	The vector that takes in each row of the training data and that is fed to dataset vector.

int attrnum, attrval;
// 	The Attribute number ranging from 1-6 and the corresponsing values that each of the attributes can take.
/*
0: 0 1
1: 1 2 3
2: 1 2 3
3: 1 2
4: 1 2 3
5: 1 2 3 4
6: 1 2
*/

float es, maxgains;
// 	Helper Variables that are used in finding Entropy and also for holding the max gain that has been found for all attributes

float gains[7];
// 	A Float array holding the value of Gains for the Attributes. Gains[0]=0 as set in the code.

int width=0;
// A helper variable that is used for formattings to understand the structure of the tree if needed to be printed.


//************************STRUCTURE HOLDING THE NODE IN THE TREE*****************************
//*******************************************************************************************
struct node
{
	string value; //if it is a leaf node we need to check if it is a true or a false or a NULL node.
	int type; // to find whether it is an internal or an external node
	int id;	// to uniquely identify the attribute.
	struct node *array[10];  //to hold the pointers of the item.

};


//********************A FUNCTION THAT HELPS IN CREATION OF NEW NODE*************************
//******************************************************************************************
struct node* createnode()
{
	//cout<<"Came to CreateNode()"<<endl;
	struct node* temp=(struct node*)malloc(sizeof(struct node));
	return temp;
}


//******UTILITY FUNCTION USED ONLY DURING DEBUGGING TO PRINT ANY VECTOR OF VECTOR***********
//******************************************************************************************
void print(vector< vector<int> > & v)
{
	for (int i = 0; i < v.size(); ++i)
	{
		cout<<i<<": ";
		for (int j = 0; j < v[i].size(); ++j)
		{
			cout<<v[i][j]<<" ";
		}
		cout<<"\n";
	}
}


//****************************PUT TO VECTOR UTILITY****************************************
//*****************************************************************************************
//DESC:
/*
		A Function that converts attrs from being a vector of sets to a vector of vector for
		easy manipulations later on during the course of the program, as Vectors are easier
		to handle and manipulate.	(Vectors are also Faster)
*/
void put_to_vector()
{
	for(int i=0;i<attrs.size();i++)
	{
		attrs_temp.clear();
		std::copy(attrs[i].begin(),attrs[i].end(),std::back_inserter(attrs_temp));
		attrs_new.push_back(attrs_temp);
	}
		//print(attrs_new);
}


//*********************************INPUT DATA FUNCTION*************************************
//*****************************************************************************************
/*
		A Function that takes input of Training data and stores the values in "dataset". It also
		populates the attrs vector here. Sets automatically remove duplicates and sort the elements
		they hold, this was very desirable in determing the various values that the particular
		attribute could hold.
*/
void inputData()
{
	ifstream input("trainingdata.txt");
	int temp;
	for (int i = 0; i < 7; ++i)
	{
		/* creating 7 sets inside attrs vector to hold unique attribute values*/
		attrs.push_back(values);
	}
	for (int i = 0; i < 124; ++i)
	{
		data.clear(); // Clearing the helper vector data.
		string cl;
		input>>cl;	// Clearing the first label of the input.
		for (int j = 1; j < 7; ++j)
		{
			/* 7 is no. of attrs Target attribute included. */
			input>>temp;
			data.push_back(temp);
			attrs[j].insert(temp);

		}
		it=data.begin();
		input>>temp;
		data.insert(it,temp);
		attrs[0].insert(temp);

		dataset.push_back(data);
		dstemp.push_back(data);
		ds.push_back(data);
		// dstemp and ds are the duplicates that are stored currently for use later on.
	}
		// print(dataset);
		put_to_vector();

		//cout<<"\n RESULT: \n Size of Dataset="<<dataset.size()<<"\n";
		//cout<<"No. Of Attributes="<<attrs.size()<<"\n"<<"\n";
}


//**********************************NEW DATASET2*******************************************
//*****************************************************************************************
/*
			A Function that takes in a dataset(mostly reduced after multiple attributes being removed from the set)
			and returns a new dataset containing only the attribute values for the attribute that has been requested
			for in the comments.
*/
vector< vector<int> > newDataSet2(vector< vector<int> >  v, int attrnum, int attrval)
{
	ds2.clear();
	for (int i = 0; i < v.size(); ++i)
	{
		if (v[i][attrnum] == attrval)
		{
			ds2.push_back(v[i]);
		}
	}
	return ds2;
}


//**********************************NEW DATSET**********************************************
//******************************************************************************************
/*
			A Function to return the number of rows that match to the given attrval of a particular
			attribute. A helper function that is used to give back values to entropy calculations.
*/

float newDataSet(vector< vector<int> > & v, int attrnum, int attrval)
{
	ds1.clear();
	float a =0;
	for (int i = 0; i < v.size(); ++i)
	{
		if (v[i][attrnum] == attrval)
		{
			ds1.push_back(v[i]);
		}
	}

	a = ds1.size();
	return a;
}


//************************************CALC ENTROPY****************************************
//****************************************************************************************
/*
		A Function that calculates the Entropy of a Particular set that is fed as input to it.
*/
float calcEntropy(vector< vector<int> > & v)
{

	float e=0;
	float a0 = newDataSet(v, 0,0);
	float a1 = newDataSet(v, 0,1);
	//	cout<<"\n No="<<a0<<" Yes="<<a1<<" Total="<<v.size()<<"\n \n";
	if (a0 == 0 || a1 ==0)
		return 0;
	e = -( ( (a0/v.size()) * (log(a0/v.size())/log(2)) ) + ( (a1/v.size()) * (log(a1/v.size())/log(2)) ) );
	//cout<<e;
	return e;
}


//********************************CALCULATE GAIN FUNCTION********************************
//***************************************************************************************
/*
		A Function that Returns Gain calculated for a DatSet and for a required Attribute as
		passed in the arguments.
*/
float calcGain(vector< vector<int> > & v, int attrnum)
{
	float g;
	float calc=0;
	for (int i = 1; i <= attrs[attrnum].size(); ++i)
	{
		float gaina = newDataSet(v, attrnum, i);
		ds.clear();
		for (int o=0; o<ds1.size(); o++)
		{
			ds.push_back(ds1[o]);
		}
		float ab = calcEntropy(ds);
		//cout<<"Size of S New ="<<gaina<<" Size of S Prev="<<v.size()<<" "<<" ";
		calc += (gaina/v.size())*ab;
		//cout<<calc<<"\n";
	}
	g = calcEntropy(v) - calc;
	//cout<<"GAINS="<<g<<"\n \n";
	return g;
}

//***********************************MAXGAIN FUNCTION***********************************
//**************************************************************************************
/*
		A Function that is modular component that tells the next attribute that needs to be
		selected in the tree. The one with the highest gain is the one that is selected and
		sued in the recursive ID3 call. It calculates the gains of all the attributes and
		returns the attribute number for the corresponding set.

*/

int maxGain(vector< vector<int> > v)
{
	gains[0] = 0;
	if(v.size()==0)
	{
		return -3;
	}
	for (int i = 1; i < 7; ++i)
	{
		/* code */
		gains[i] = calcGain(v, i);
		//cout<<"Gain of attr"<<i<< "is = > "<<gains[i]<<endl;
		//cout<<"Size of V is"<<v.size();
		//cout<<gains[i]<<" ";
	}
	//	cout<<"THe gain"<<calcGain(v,6)<<endl;

	int flag = 1;
	maxgains = gains[1];
	/* Function to calc max of all gains */
	for (int i = 1; i < 7; ++i)
	{
		if(gains[i]>maxgains)
		{
			maxgains=gains[i];
			flag = i;
		}
	}
	//print(ds1);

	if (maxgains == 0)
	{
		//cout<<"reached here"<<endl;
		if(ds2.size() == 0)
			{return -3;}
		if(ds2[0][0]==0)
			{return -2;}
		if(ds2[0][0]==1)
			{return -1;}
	}
	return flag;
}


//************************************ID3 RECURSIVE FUNCTION********************************
//*******************************************************************************************
/*
		The Recursive ID3 Function that makes use of all the components discussed above to generate
		the Decision tree. Each node is Designed as an attroibute and it has ponters to all the
		attribute values that it can take.
		It takes in the Following Inputs.
		1) A Reduced data Set.
		2) The node pointer.
		3) A Vector Print that helps us to print the rules.
		4) A n_t variable to help us with tracing the DFS calls that we perform to build the tree.
*/
void id3(vector <vector<int> > the_set,struct node* temp,int width,vector <int> print,int n_t)
{
	//cout<<"came here"<<"\n";
	int tt=0;
	int id_max=maxGain(the_set);
	// id_max has the attribute that resulted in the maximum gain for the particular set(the_set)
	//cout<<endl;
	//cout<<" THE INDEX";
	//cout<<setw(width)<<id_max<<"\n";  // If Uncommented will give a Rough structure of how the Decision tree looks like
	//int id_max=aa[l];
	//l++;
	//	vector<int> temp_vec;
	if(n_t==0)
	{
		//To handle the initial Case.
		print.push_back(id_max);
	}
	else
	{
			print.push_back(n_t);
			print.push_back(id_max);
	}

	temp->id=id_max;
	int n=0;
	if(id_max>0)
	{
		n=attrs_new[id_max].size();
	}
	if(id_max==-1)
	{
		// THE CASE OF THE LEAF NODE BELONGINF TO TARGET CLASS 1
		for(int j=0;j<print.size()-1;j++)
			{
				if(j==0)
					output_rules<<"IF Att_"<<print[j]<<" = ";
				else if(j%2==0) //printing the attribute
					output_rules<<"  and Att_"<<print[j]<<" = ";
				else
					output_rules<<print[j];

			}

			output_rules<<" THEN => 1 is target class"<<endl;
	}
	else if(id_max==-2)
	{
		// THE CASE OF THE LEAF NODE BELONGING TO TARGET CLASS 2
		for(int j=0;j<print.size()-1;j++)
			{
					if(j==0)
					output_rules<<"IF Att_"<<print[j]<<" = ";
				else if(j%2==0) //printing the attribute
					output_rules<<"  and Att_"<<print[j]<<" = ";
				else
					output_rules<<print[j];
			}

			output_rules<<" THEN => 0 is target class"<<endl;
	}
		for(int i=0;i<n;i++)
		{
		(temp->array)[i]=createnode();
		temp=(temp->array)[i];
		//the function to create the newset.

		//the first is the original set
		//the second is the attribute that needs to be used.
		//the third is the value of the attribute that needs to be used.
		//these are the three attributes to return a new set(in a vector)

		//HERE A Call to NEWDataSet2 returns a condensed data set stripping the original
		//one of the attribute that we have just selected=> id_max;
		id3(newDataSet2(the_set,id_max,attrs_new[id_max][i]),temp,width+3,print,i+1);
		//cout<<"back success"<<"value of i is "<<i<<"value of id is "<<id_max<<endl;
	}

}

//*******************************CALCULATE ACCURACY***************************************
//****************************************************************************************
/*
			The Function takes Input of The Testing set, and the pointer to the Decision tree
			that we have generated. It traverses the tree till it reaches a leaf node marked by
			either -1 or -2. Once it does it reach a leaf it checks if the predicted target value
			is same as the target calue that is derived from the tree. If so it adds to the counter.
*/
float calculate_accuracy(vector <vector <int> > vec,struct node* root)
{
	float accuracy;
	float denom=vec.size();
	float counter=0;
	struct node* temp=(struct node*)malloc(sizeof(struct node));
	for(int i=0;i<vec.size();i++)
	{

		temp=root;
		while((temp->id)>0)
		{
			//cout<<temp->type<<"the temp type\n";
			int i_d=temp->id;
			//cout<<i_d<<"we want "<<i<<"\n";
			if(i_d<0)
			{
				break;
			}
			int ii=vec[i][i_d];
			ii--;
			temp=(temp->array)[ii];

		}
		if((temp->id==-1 && vec[i][6]==1) || (temp->id==-2 && vec[i][6]==0))
		{
			counter++;
		}
	}
	accuracy=(counter/denom);
	return accuracy;
}

//********************************TEST DAT INPUT*************************************
//***********************************************************************************
/*
		A Helper function to read the testdata from testingdata.txt.The input stream is
		created and input read from there. It returns a Vector of Vector with all the
		rows of the testing data in a pattern similar to the training data.
*/
vector <vector <int> >  testdata()
{
	ifstream inputtest("testingdata.txt");
	vector <vector <int> > full_test;
	vector <int> test_container;
	for(int i=0;i<432;i++)
	{
		string s;
		inputtest>>s;
		for(int i=0;i<7;i++)
		{
			int temp;
			inputtest>>temp;
			test_container.push_back(temp);
		}
		full_test.push_back(test_container);
	}
	return(full_test);
}


//***********************************DRIVER MAIN*****************************************
//***************************************************************************************
/*
			The driver function that takes input and initiates the Building of the Decision tree.
*/
int main()
{

	ofstream outputacc("accuracy.txt");
	inputData();
	//Building the Input training Set.
	//es = calcEntropy(dstemp); // to check the correctness of calculate entropy,

	struct node* root=(struct node*)malloc(sizeof(struct node));
	// Root of the decision tree.

	vector <int> print;
	//Helper vector if tree is to be displayed in s layered format.

	id3(dstemp,root,1,print,0);
	//Call to ID3 that starts beuilding of the Decision tree.

	float acc=calculate_accuracy(testdata(),root);
	outputacc<<"ACCURACY OF THE CLASSIFICATION ON THE TRAINING DATA IS: "<<acc*100<<"%";
	//OUTPUT accuracy of the training Data is written to accuracy.txt file in the same folder.
	return 0;
}
