#pragma once
#include <IGraphicsBuffer.h>
#include <IDisposable.h>

class StencilBuffer : public IGraphicsBuffer, public IDisposable
{
private:
	std::vector<int> passIDs;
	std::string parent;
	std::string target;
	std::string name;

public:
	StencilBuffer();
	~StencilBuffer();

	const std::string GetName();
	void SetName(const std::string& name);
	const unsigned long long GetTypeID();
	const std::string GetType();
	void SetTarget(const std::string& gameObject);
	const std::string GetTarget();
	void SetParent(const std::string& parent);
	const std::string GetParent();
	const bool AddPassID(const int& passID);
	const bool RemovePassID(const int& passID);
	const std::vector<int> GetPassIDs();
	void Dispose();
};

