#pragma once


class Multikey
{
public:
	long  key1;
	int   key2;
	long  key3;
	long  key4;

	Multikey(long k1, int k2, long k3, long k4)
		: key1(k1), key2(k2), key3(k3), key4(k4) {}

	bool operator<(const MultiKey &right) const 
    {
        if ( key1 == right.key1 ) 
		{
            if ( key2 == right.key2 ) 
			{
                if ( key3 == right.key3 ) 
				    return key4 < right.key4;
                else
                    return key3 < right.key3;
                
            }
            else
				return key2 < right.key2;
        }
        else 
            return key1 < right.key1;
       
    }    
	bool operator==(const MultiKey &right) const
	{
		return key1 == right.key1 && key2 == right.key2 && key3 == right.key3 && key4 == right.key4;
	}
};