
#include "FakeVmsConnector.hpp"

FakeVmsConnector::FakeVmsConnector() : opencctv::api::VmsConnector() {
	_lCount = 0;
	_pVImageData = NULL;
	_pConfig = NULL;
}

bool FakeVmsConnector::init(const opencctv::api::VmsConnectInfo& info, const std::string sPathToAnalyticDir)
{
	bool bRet = false;
	std::string sConfigFilePath = sPathToAnalyticDir;
	sConfigFilePath.append("/").append("analytic.config");
	try
	{
		_pConfig = util::Config::getInstance(sConfigFilePath);
	}
	catch(std::runtime_error &e)
	{
		std::cerr << "Failed to read Analytic Configuration file. " << e.what() << std::endl;
	}
	if(_pConfig)
	{
		std::string sFilePath = sPathToAnalyticDir;
		sFilePath.append("/").append(_pConfig->get(util::PROPERTY_IMAGE_FILE));
		std::vector<char>* pVImageData = NULL;
		try {
			std::ifstream imageFile(sFilePath.c_str(), std::ios::binary | std::ios::ate);
			std::ifstream::pos_type pos = imageFile.tellg();
			pVImageData = new std::vector<char>(pos);
			imageFile.seekg(0, std::ios::beg);
			imageFile.read(&(*pVImageData)[0], pos);
			_pVImageData = new std::vector<unsigned char>(pVImageData->begin(), pVImageData->end());
			bRet = true;
		} catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
		}
		if(pVImageData) delete pVImageData;
	}
	return bRet;
}

void FakeVmsConnector::produceImageObjects(opencctv::ConcurrentQueue<opencctv::Image>* pQueue)
{
	if(_pConfig && _pVImageData) _bEnable = true;
	while(_bEnable)
	{
		opencctv::Image* pImage = new opencctv::Image();
		std::stringstream ssTimestamp;
		ssTimestamp << "Custom Timestamp ";
		ssTimestamp << _lCount;
		pImage->setTimestamp(ssTimestamp.str());
		pImage->setWidth(boost::lexical_cast<unsigned int>(_pConfig->get(util::PROPERTY_IMAGE_WIDTH)));
		pImage->setHeight(boost::lexical_cast<unsigned int>(_pConfig->get(util::PROPERTY_IMAGE_HEIGHT)));
		pImage->setImageData(*_pVImageData);
		pQueue->push(pImage);
		_lCount++;
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	}
	if(_pVImageData) delete _pVImageData;
}

bool FakeVmsConnector::isStillProducingImages()
{
	return _bEnable;
}

FakeVmsConnector::~FakeVmsConnector() {
	if(_pVImageData) delete _pVImageData;
}

