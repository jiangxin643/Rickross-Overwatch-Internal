#pragma once

class c_tank {
public:
	void think();
	
	void rein_think();
	void block_think();
	
	bool _rein_ult_cast;
	float _rein_ult;
	float _zarya_ult;
	
	float _junk_shift;
	float _sombra_e;
	float _zen_m2;
};

extern c_tank* g_tank;