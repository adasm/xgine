/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#pragma once
#include "XGine.h"

/*** Tokenizer ***/
typedef struct XGINE_API Tokenizer 
{
private:
	u32		current_pos;
	u32		buffer_size;
	u32		old_pos, ts, te;
	bool	tq;
	string	buffer;
public:
	Tokenizer() : buffer(""), current_pos(0), buffer_size(0) { }; 
	Tokenizer(string buff) : buffer(buff), current_pos(0), buffer_size(buff.length()) { }; 
	Tokenizer(string buff, u32 size) : buffer(buff), current_pos(0), buffer_size(size) { }; 
	Tokenizer(c8 *buff, u32 size) : buffer(string(buff,size)), current_pos(0), buffer_size(size) { }
	~Tokenizer() { }

	void setBuff(string buff) { buffer = buff; current_pos = 0; buffer_size = buff.length(); }
	void setBuff(string buff, u32 size) { buffer = buff; current_pos = 0; buffer_size = size; }
	void setBuff(c8 *buff, u32 size) { buffer = string(buff,size); current_pos = 0; buffer_size = size; }

	void clear(string comments);
	bool nextToken(string *gtoken);
	bool lastQuoted() { return tq; }
	void setPos(u32 pos) { current_pos = (pos < buffer_size)? (pos) : (buffer_size-1); }
}*Tokenizer_ptr;