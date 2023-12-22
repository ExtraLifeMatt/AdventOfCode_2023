// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"

#include "ACUtils/Algorithm.h"
#include "ACUtils/Hash.h"
#include "ACUtils/Math.h"
#include "ACUtils/StringUtil.h"
#include <vector>
#include <cinttypes>
#include <queue>

enum class ElfSignal : uint8_t
{
	ES_Low = 0,
	ES_High
};

class ElfModule;
struct SignalMessage
{
	SignalMessage(ElfModule* _source, ElfModule* _dest, ElfSignal _signal):Source(_source), Destination(_dest), Signal(_signal){};
	ElfModule* Source;
	ElfModule* Destination;
	ElfSignal Signal;
};

class ElfModule
{
public:
	virtual void Execute(const SignalMessage& SignalMsg) = 0;
	void AddOutputModule(ElfModule* output) { m_Outputs.emplace_back(output); }
	const std::vector<ElfModule*>& GetOutputModules() const { return m_Outputs; }
	void ConnectTo(ElfModule* Output)
	{
		m_Outputs.push_back(Output);
		Output->OnInputConnected(this);
	}
	const std::string& GetName() const { return m_Name; }
	virtual void ResetState() {};

	template<class T>
	const T* GetAs() const
	{
		return static_cast<const T*>(this);
	}
protected:
	ElfModule(const std::string& name, std::queue<SignalMessage>* signalQueue):m_Name(name), m_SignalQueue(signalQueue) {}
	virtual ~ElfModule() {};

	virtual void OnInputConnected(ElfModule* NewInput) { m_Inputs.push_back(NewInput); }
	void QueueToAllOutputs(ElfSignal signal) 
	{ 
		for (ElfModule* output : m_Outputs)
		{
			m_SignalQueue->emplace(this, output, signal);
		}
	}
	void QueueSignal(ElfModule* module, ElfSignal signal) { m_SignalQueue->emplace(this, module, signal); }
	
	std::queue<SignalMessage>* m_SignalQueue;
	std::vector<ElfModule*> m_Outputs;
	std::vector<ElfModule*> m_Inputs;
	std::string m_Name;
};

class FlipFlop : public ElfModule
{
public:
	FlipFlop(const std::string& name, std::queue<SignalMessage>* signalQueue):ElfModule(name, signalQueue), m_State(false) {}
	virtual void Execute(const SignalMessage& SignalMsg) override
	{
		if (SignalMsg.Signal == ElfSignal::ES_High)
		{
			return;
		}

		m_State = !m_State;

		QueueToAllOutputs(m_State ? ElfSignal::ES_High : ElfSignal::ES_Low);
	}

	virtual void ResetState() override { m_State = false; }
private:
	bool m_State;
};

class Conjunction : public ElfModule
{
public:
	Conjunction(const std::string& name, std::queue<SignalMessage>* signalQueue) :ElfModule(name, signalQueue) {}
	virtual void Execute(const SignalMessage& SignalMsg) override
	{
		for (size_t i = 0; i < m_History.size(); ++i)
		{
			if (m_History[i].Source == SignalMsg.Source)
			{
				m_History[i].Signal = SignalMsg.Signal;
				break;
			}
		}

		bool allHighSignals = true;
		for (const SignalMessage& history : m_History)
		{
			if (history.Signal != ElfSignal::ES_High)
			{
				allHighSignals = false;
				break;
			}
		}

		QueueToAllOutputs(allHighSignals ? ElfSignal::ES_Low : ElfSignal::ES_High);
	}

	virtual void ResetState() override 
	{
		for (SignalMessage& msg : m_History)
		{
			msg.Signal = ElfSignal::ES_Low;
		}
	}
protected:
	virtual void OnInputConnected(ElfModule* NewInput) { m_History.emplace_back(NewInput, this, ElfSignal::ES_Low); ElfModule::OnInputConnected(NewInput); }
private:
	std::vector<SignalMessage> m_History;
};

class Capture : public ElfModule
{
public:
	Capture(const std::string& name, std::queue<SignalMessage>* signalQueue) :ElfModule(name, signalQueue) {}
	virtual void Execute(const SignalMessage& SignalMsg) override
	{
		m_SignalCounts[static_cast<uint8_t>(SignalMsg.Signal)]++;
	}

	uint32_t GetLowCount() const { return m_SignalCounts[0]; }
	uint32_t GetHighCount() const { return m_SignalCounts[1]; }

	virtual void ResetState()
	{
		m_SignalCounts[0] = 0;
		m_SignalCounts[1] = 0;
	}
private:
	uint32_t m_SignalCounts[2];
};

class Broadcaster : public ElfModule
{
public:
	Broadcaster(const std::string& name, std::queue<SignalMessage>* signalQueue) :ElfModule(name, signalQueue) {}
	virtual void Execute(const SignalMessage& SignalMsg) override
	{
		QueueToAllOutputs(SignalMsg.Signal);
	}
};

struct GetModuleByNamePred
{
	GetModuleByNamePred(const std::string& name) : m_Name(name){};
	bool operator()(const ElfModule* module) const
	{
		return module->GetName() == m_Name;
	}
	std::string m_Name;
};

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params)
		: AdventGUIInstance(params)
	{};
private:
	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		std::vector<std::string> tokens;

		// First pass, create all our modules.
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			assert(line.size() != 0);
			size_t spaceIdx = line.find(' ');
			assert(spaceIdx != std::string::npos);
			if (line[0] == '%' || line[0] == '&')
			{
				ElfModule* newModule = nullptr;
				if (line[0] == '%')
				{
					newModule = new FlipFlop(line.substr(1, spaceIdx - 1), &m_SignalQueue);
				}
				else
				{
					newModule = new Conjunction(line.substr(1, spaceIdx - 1), &m_SignalQueue);
				}
				m_Modules.push_back(newModule);
			}
			else
			{
				ElfModule* newModule = new Broadcaster(line.substr(0, spaceIdx), &m_SignalQueue);
				m_Modules.push_back(newModule);
			}
		}

		fileReader.SeekAbsolute(0);
		// Second pass, setup connections.
		uint8_t sourceModuleIndex = 0;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			assert(line.size() != 0);
			size_t arrowIdx = line.find('>');
			assert(arrowIdx != std::string::npos);
			std::string connections = line.substr(arrowIdx + 1);
			StringUtil::SplitBy(connections, ",", tokens);

			for (const std::string& tok : tokens)
			{
				int32_t destModuleIndex = Algorithm::find_index_of(m_Modules.begin(), m_Modules.end(), GetModuleByNamePred(tok));
				if (destModuleIndex != -1)
				{
					m_Modules[sourceModuleIndex]->ConnectTo(m_Modules[destModuleIndex]);
				}
				else
				{
					ElfModule* newCapture = new Capture(tok, &m_SignalQueue);
					m_Modules.push_back(newCapture);
					m_Modules[sourceModuleIndex]->ConnectTo(newCapture);
				}
			}

			++sourceModuleIndex;
		}
		
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		uint32_t LowSignalCounts = 0;
		uint32_t HighSignalCounts = 0;

		for (uint32_t i = 0; i < 1000; ++i)
		{
			// Start Message
			int32_t destModuleIndex = Algorithm::find_index_of(m_Modules.begin(), m_Modules.end(), GetModuleByNamePred("broadcaster"));
			assert(destModuleIndex != -1);

			m_SignalQueue.emplace(nullptr, m_Modules[destModuleIndex], ElfSignal::ES_Low);
			SignalMessage currentSignal(nullptr, nullptr, ElfSignal::ES_Low);
			while (!m_SignalQueue.empty())
			{
				currentSignal = m_SignalQueue.front();
				m_SignalQueue.pop();

				if (currentSignal.Signal == ElfSignal::ES_Low)
				{
					++LowSignalCounts;
				}
				else
				{
					++HighSignalCounts;
				}

				currentSignal.Destination->Execute(currentSignal);
			}
		}

		Log("Total Pulses: %llu", (uint64_t)LowSignalCounts * (uint64_t)HighSignalCounts);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{	
		// Part Two
		for (ElfModule* module : m_Modules)
		{
			module->ResetState();
		}

		int32_t rxModuleIndex = Algorithm::find_index_of(m_Modules.begin(), m_Modules.end(), GetModuleByNamePred("rx"));
		assert(rxModuleIndex != -1);

		const Capture* rxModule = m_Modules[rxModuleIndex]->GetAs<Capture>();
		uint32_t totalButtonPresses = 0;

		uint32_t lastUpdateValueForConj[4] = { 0 };
		uint32_t buttonPressesForConj[4] = { 0 };
		const char* conjModuleNames[] = {"kd", "zf", "vg", "gs"}; // Found by looking at the RX module inputs (1 conj, which has inputs from 4 conj, these guys).
		bool stableNumbers[4] = { false }; 
		const Conjunction* watchModules[4] = { nullptr };

		for (uint32_t i = 0; i < ARRAY_SIZE(conjModuleNames); ++i)
		{
			int32_t watchIdx = Algorithm::find_index_of(m_Modules.begin(), m_Modules.end(), GetModuleByNamePred(conjModuleNames[i]));
			assert(watchIdx != -1);
			watchModules[i] = m_Modules[watchIdx]->GetAs<Conjunction>();
		}

		uint32_t totalSignalsSent = 0;
		bool canBreak = false;
		while(!canBreak)
		{
			// Start Message
			int32_t destModuleIndex = Algorithm::find_index_of(m_Modules.begin(), m_Modules.end(), GetModuleByNamePred("broadcaster"));
			assert(destModuleIndex != -1);

			m_SignalQueue.emplace(nullptr, m_Modules[destModuleIndex], ElfSignal::ES_Low);
			SignalMessage currentSignal(nullptr, nullptr, ElfSignal::ES_Low);

			totalSignalsSent = 0;

			while (!m_SignalQueue.empty())
			{
				currentSignal = m_SignalQueue.front();
				m_SignalQueue.pop();
				++totalSignalsSent;

				for (uint32_t i = 0; i < ARRAY_SIZE(watchModules); ++i)
				{
					// Update the conjunction modules we're watching.
					if (currentSignal.Source == watchModules[i] && currentSignal.Signal == ElfSignal::ES_High)
					{
						Log("Conjunction Module %s sent a High signal on press [%u] step [%u]", currentSignal.Source->GetName().c_str(), totalButtonPresses, totalSignalsSent);

						uint32_t oldValue = buttonPressesForConj[i];
						buttonPressesForConj[i] = totalButtonPresses - lastUpdateValueForConj[i];
						stableNumbers[i] = oldValue == buttonPressesForConj[i];
						lastUpdateValueForConj[i] = totalButtonPresses;

						// If we have stable numbers for all 4, we can stop.
						canBreak = true;
						for (uint32_t j = 0; j < 4; ++j)
						{
							canBreak &= stableNumbers[i];
						}
					}
				}

				currentSignal.Destination->Execute(currentSignal);
			}

			++totalButtonPresses;
		}

		Log("Stable after %u presses. Counts [%u] [%u] [%u] [%u].", totalButtonPresses, buttonPressesForConj[0], buttonPressesForConj[1], buttonPressesForConj[2], buttonPressesForConj[3]);

		uint64_t lcmValueA = Math::LCM(buttonPressesForConj[0], buttonPressesForConj[1]);
		uint64_t lcmValueB = Math::LCM(buttonPressesForConj[2], buttonPressesForConj[3]);

		uint64_t lcm = Math::LCM(lcmValueA, lcmValueB);

		Log("Total Button Presses: %llu", lcm);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	std::queue<SignalMessage> m_SignalQueue;
	std::vector<ElfModule*> m_Modules;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 20;
	newParams.year = 2023;
	newParams.puzzleTitle = "Pulse Propagation";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
