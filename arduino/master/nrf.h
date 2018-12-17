
#ifndef __LOCALHOST_NRF_H__
#define __LOCALHOST_NRF_H__

class NRF {
private:
	bool isMaster;
public:
	void setup(uint32_t nodeId, bool isMaster);
	void loop();
};

#endif // __LOCALHOST_NRF_H__
