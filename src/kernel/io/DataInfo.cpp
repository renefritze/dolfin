#include <iostream>

#include "DataInfo.h"
#include <dolfin/constants.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
DataInfo::DataInfo(const char *description, int no_data, int *dimensions)
{
  this->no_data = no_data;
  
  labels            = new (char *)[no_data];
  names             = new (char *)[no_data];
  this->dimensions  = new int[no_data];
  this->description = new char[DOLFIN_LINELENGTH];
  
  for (int i=0;i<no_data;i++){
	 labels[i] = new char[DOLFIN_LINELENGTH];
	 names[i]  = new char[DOLFIN_LINELENGTH];
	 sprintf(labels[i],"unnamed data <%d>",i+1);
	 if ( no_data == 1 )
		sprintf(names[i],"u");
	 else
		sprintf(names[i],"u%d",i);
	 this->dimensions[i] = dimensions[i];
  }

  sprintf(this->description,description);
}
//-----------------------------------------------------------------------------
DataInfo::~DataInfo()
{
  delete [] dimensions;
  dimensions = 0;
  
  delete [] description;
  description = 0;

  for (int i=0;i<no_data;i++){
	 delete [] labels[i];
	 labels[i] = 0;
	 delete [] names[i];
	 names[i] = 0;
  }

  delete [] labels;
  labels = 0;
  delete [] names;
  names = 0;
}
//-----------------------------------------------------------------------------
void DataInfo::SetLabel(int i, const char *name, const char *label)
{
  if ( (i < 0) || (i >= no_data) ) {
	 cout << "DataInfo::SetLabel(): Data index out of range." << endl;
	 exit(1);
  }

  sprintf(names[i],name);
  sprintf(labels[i],label);
}
//-----------------------------------------------------------------------------
int DataInfo::Size()
{
  return no_data;
}
//-----------------------------------------------------------------------------
int DataInfo::Dim(int i)
{
  if ( (i < 0) || (i >= no_data) ) {
	 cout << "DataInfo::SetLabel(): Data index out of range." << endl;
	 exit(1);
  }
  
  return dimensions[i];
}
//-----------------------------------------------------------------------------
const char *DataInfo::Description()
{
  return description;
}
//-----------------------------------------------------------------------------
const char *DataInfo::Label(int i)
{
  if ( (i < 0) || (i >= no_data) ) {
	 cout << "DataInfo::SetLabel(): Data index out of range." << endl;
	 exit(1);
  }

  return labels[i];
}
//-----------------------------------------------------------------------------
const char *DataInfo::Name(int i)
{
  if ( (i < 0) || (i >= no_data) ) {
	 	 cout << "DataInfo::SetLabel(): Data index out of range." << endl;
	 exit(1);
  }

  return names[i];
}
//-----------------------------------------------------------------------------
