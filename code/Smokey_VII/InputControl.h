#ifndef _INPUT_CONTROL_H_
#define _INPUT_CONTROL_H_

class Timer;

class ToggleLatch
{
public:
	ToggleLatch(double timeBetween=0):time_(), timeBetween_(timeBetween), value_(false){}
	inline bool Get(void) const {return value_;}
	void Toggle(void);
	void Init(void);
	void Disable(void);
private:
	Timer time_;
	double timeBetween_;
	bool value_;
};

#endif //_INPUT_CONTROL_H_
