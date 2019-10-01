#include "Schedule.hpp"
#include "Resources.hpp"

namespace Resources = XmdsResources::Schedule;

Soap::RequestSerializer<Schedule::Request>::RequestSerializer(const Schedule::Request& request) :
    BaseRequestSerializer(request)
{
}

std::string Soap::RequestSerializer<Schedule::Request>::string()
{
    return createRequest(Resources::Name, request().serverKey, request().hardwareKey);
}

Soap::ResponseParser<Schedule::Result>::ResponseParser(const std::string& soapResponse) :
    BaseResponseParser(soapResponse)
{
}

Schedule::Result Soap::ResponseParser<Schedule::Result>::parseBody(const PtreeNode& scheduleNode)
{
    Schedule::Result result;
    result.scheduleXml = scheduleNode.get<std::string>(Resources::ScheduleXml);
    return result;
}
