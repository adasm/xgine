/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#include "Tokenizer.h"

void Tokenizer::clear(string comments)
{
	string output = "", line; 
	for(u32 i = 0; i < buffer.size(); i++)
	{
		if(comments.find(buffer[i]) != string::npos)
			while(i < buffer.size() && buffer[i] != '\n')i++;
		else output += buffer[i];
	}
	buffer = output;
	buffer_size = output.length();
}

bool Tokenizer::nextToken(string *gtoken)
{
	static c8 startc[] = "\n\t\r "; //4
	static c8 endc[] = "\n\t\r \";=,"; //12
	static c8 charswhited[] = "(){};="; //6
	static u32 i;
	old_pos = current_pos;

	for(; current_pos<=buffer_size; current_pos++){
		for(i=0; i<4; i++){				
			if(buffer[current_pos] == startc[i]){
				goto con;
			}				
		}	
		ts = current_pos;
		current_pos++;
		goto p2;
	 con:
		continue;
	}
	return false;
	p2:
	for(i=0; i<6; i++){
		if(buffer[ts] == charswhited[i]){
			te=ts;
				(*gtoken) = string(&buffer[ts], te-ts+1);
			return true;
		}
	}
	if(buffer[ts] == '\"'){
		ts++;
		for(; current_pos<buffer_size; current_pos++){
			if(buffer[current_pos] == '\"')	{
				te = current_pos-1; tq = true;						
				current_pos++;
				if(te<ts) (*gtoken) = std::string("");
				else (*gtoken) = string(&buffer[ts], te-ts+1);
				return true;
			}
		}
	}
	else if(buffer[ts] == '\''){
		ts++;
		for(; current_pos<buffer_size; current_pos++){
			if(buffer[current_pos] == '\'')	{
				te = current_pos-1; tq = true;						
				current_pos++;
				if(te<ts) (*gtoken) = std::string("");
				else (*gtoken) = string(&buffer[ts], te-ts+1);
				return true;
			}
		}
	}
	else{
		for(; current_pos<buffer_size; current_pos++){
			for(i=0; i<12; i++){				
				if(buffer[current_pos] == endc[i]){
					te = current_pos-1; tq = false;	
					if(te<ts)return false;
					else (*gtoken) = string(&buffer[ts], te-ts+1);
					return true;
				}				
			}
		}
		te = buffer_size-1;
		if(te<ts)return false;
		else (*gtoken) = string(&buffer[ts], te-ts+1);
		return true;
	}
	return false;
}