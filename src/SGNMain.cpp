//sgn
#include <iostream>
#include <memory>

#include "RESTful.h"
#include "DBInterface.h"
#include "MyDateTime.h"

int main() {
    DBInterface::Ptr pDBInterface = DBInterface::Ptr(DBInterface::getInstance("/home/tstone10/sgnHiringAsst/sgnHiringServer/sgnCandidates.db"));

    RESTful *pRestful = new RESTful(8080, pDBInterface);
    pRestful->run();
}

/*int main() {
    std::cout << "SGN" << std::endl;
    //DBInterface* pDBInterface = DBInterface::getInstance("/home/tstone10/sgnHiringAsst/sgnHiringServer/sgnHiring.db");

    //User::Ptr pUser = User::parseUser("{\"address\":\"B6 105\",\"dob\":1662918550,\"email\":\"test@gmail.com\",\"last_visited\":1662918550,\"membership_no\":1001,\"mobile\":\"9898989898\",\"name\":\"sangar\",\"photo\":\"/home/tstone10/sgn/proj/gym/build\",\"validity_end\":1662918550}");
    //pDBInterface->addNewUser(pUser);

    //User::Ptr pUser = pDBInterface->getUser(1001);
    //if(pUser) std::cout << pUser->toJson() << std::endl;

    //pDBInterface->markAttendance(1001);
    MyDateTime::Ptr pDate = std::make_shared<MyDateTime>();
    std::cout << pDate->getDateStr() << ", " << pDate->getTimeStr() << std::endl;

    return 0;
}*/
