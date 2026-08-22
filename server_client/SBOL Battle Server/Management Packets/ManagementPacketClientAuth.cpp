#include <Windows.h>
#include "..\server.h"

uint16_t startPositions[][2]{
	//{ 0x0022, 0x0200 },
	//#ifndef _DEBUG
	//{ 0x0023, 0x0100 },
	//{ 0x0029, 0x0366 }, // Testing
	{ 0x0024, 0x0003 }, // Main Start
	//{ 0x01CD, 0x0003 }, // Outer C1 Start
	//{ 0x0025, 0x0100 },
	//{ 0x0026, 0x0100 },
	//{ 0x0027, 0x0100 },
	//{ 0x0028, 0x0100 },
	//{ 0x0029, 0x0400 },
	//{ 0x002b, 0x0300 },
	//{ 0x002d, 0x0100 },
	//{ 0x002e, 0x0100 },
	//{ 0x002f, 0x0400 },
	//{ 0x0030, 0x0400 },
	//{ 0x003a, 0x0800 },
	//{ 0x003b, 0x0800 },
	//{ 0x0043, 0x0000 },
	//{ 0x0044, 0x0000 },
	//{ 0x0045, 0x0000 },
	//{ 0x004e, 0x0000 },
	//{ 0x004f, 0x0000 },
	//{ 0x0056, 0x0000 },
	//{ 0x005a, 0x0000 },
	//{ 0x005b, 0x0000 },
	//{ 0x0063, 0x0000 },
	//{ 0x0066, 0x0000 },
	//{ 0x006f, 0x0000 },
	//{ 0x00DC, 0x0480 } // Team Center
	//#endif
};

void ManagementPacketClientAuth(Client* client)
{
	Server* server = (Server*)client->server;
	switch (client->serverbuf.getSubType())
	{
	case 0x0000: // Authentication
	{
		client->serverbuf.setOffset(0x0A);
		int32_t license = client->serverbuf.get<int32_t>();
		if (license == -1)
		{
			client->SendAuthError(Server::AUTHLIST::AUTH_INVALID_PW);
			return;
		}
		else
		{
			uint8_t loggedIn = client->serverbuf.get<uint8_t>();
			if (loggedIn)
			{
				client->SendAuthError(Server::AUTHLIST::AUTH_LOGGED_IN);
				client->Disconnect();
				return;
			}
			else
			{
				client->server->disconnectLoggedInUser(license);
				client->driverslicense = license;
				server->addAuthenticatedClient(license);
				std::string handle = client->serverbuf.getStringA(0x10);
				client->careerdata.CP = client->serverbuf.get<int64_t>();
				client->careerdata.level = client->serverbuf.get<uint8_t>();
				client->careerdata.experiencePoints = client->serverbuf.get<uint32_t>();
				client->careerdata.level = client->getLevel();
				client->careerdata.experiencePercent = 0;
				client->careerdata.playerWin = client->serverbuf.get<uint32_t>();
				client->careerdata.playerLose = client->serverbuf.get<uint32_t>();
				client->careerdata.rivalWin = client->serverbuf.get<uint32_t>();
				client->careerdata.rivalLose = client->serverbuf.get<uint32_t>();
				client->serverbuf.getArray((uint8_t*)&client->careerdata.rivalStatus, sizeof(client->careerdata.rivalStatus));
				int8_t activeCarBay = client->serverbuf.get<int8_t>();
				client->privileges = client->serverbuf.get<uint8_t>();
				client->notBeginner = (client->serverbuf.get<uint8_t>() ? true : false);
				client->teamdata.teamID = client->serverbuf.get<uint32_t>();
				uint32_t flags = client->serverbuf.get<uint32_t>();
				client->careerdata.ranking = client->serverbuf.get<uint32_t>();
				if (flags & 1)
				{
					client->SendAuthError(Server::AUTHLIST::AUTH_DISABLED);
					client->Disconnect();
					return;
				}
				else if (flags & 128)
				{
					client->SendAuthError(Server::AUTHLIST::AUTH_DISABLED);
					client->Disconnect();
					return;
				}
				else
				{
					client->enableStandardPackets();

					// Existing-account payload from DB (only present when handle was set).
					uint8_t loadedGarageCount = 0;
					uint8_t loadedCarCount = 0;
					uint32_t teamID = 0;
					uint32_t teamLeaderID = 0;
					std::string teamLeaderName = "";
					std::string teamName = "";
					std::string teamComment = "";
					uint32_t teamSurvivalWin = 0;
					uint32_t teamSurvivalLose = 0;
					uint32_t teamCount = 0;
					uint32_t teamFlag = 0;
					uint8_t inTeam = 0;
					uint8_t teamarea = 0;

					if (handle != "")
					{
						client->initializeTeam();
						client->initializeGarage();
						client->handle = handle;

						loadedGarageCount = client->serverbuf.get<uint8_t>();
						client->garagedata.garageCount = loadedGarageCount;
						for (uint32_t i = 0; i < loadedGarageCount; i++)
							client->garagedata.garageType.push_back(client->serverbuf.get<uint8_t>());

						loadedCarCount = client->serverbuf.get<uint8_t>();
						for (uint32_t i = 0; i < loadedCarCount; i++)
						{
							uint8_t bay = client->serverbuf.get<uint8_t>();
							uint32_t carID = client->serverbuf.get<uint32_t>();
							float kms = client->serverbuf.get<float>();
							CARMODS mods = { 0 };
							PARTS parts = { 0 };
							client->serverbuf.getArray((uint8_t*)&mods, sizeof(CARMODS));
							client->serverbuf.getArray((uint8_t*)&parts, sizeof(PARTS));
							uint32_t condition = client->serverbuf.get<uint32_t>();

							if (bay >= client->garagedata.car.size())
							{
								client->logger->Log(Logger::LOGTYPE_CLIENT, L"Auth garage bay %u out of range for license %u — ignored.", bay, license);
								continue;
							}
							if (client->isValidCar(carID) == false) carID = Server::CARLIST::AE86_L_3_1985;
							client->garagedata.car[bay].carID = carID;
							client->garagedata.car[bay].KMs = kms;
							client->garagedata.car[bay].carMods = mods;
							client->garagedata.car[bay].parts = parts;
							client->garagedata.car[bay].engineCondition = condition;
						}

						uint32_t itemCount = client->serverbuf.get<uint32_t>();
						for (uint32_t i = 0; i < itemCount; i++)
						{
							uint16_t itemID = client->serverbuf.get<uint16_t>();
							if (client->isValidItem(itemID) == true) client->itembox.push_back(itemID);
						}

						client->serverbuf.getArray(&client->sign[0], sizeof(client->sign));

						inTeam = client->serverbuf.get<uint8_t>();
						if (inTeam)
						{
							teamID = client->serverbuf.get<uint32_t>();
							teamLeaderID = client->serverbuf.get<uint32_t>();
							teamLeaderName = client->serverbuf.getStringA(0x10);
							teamName = client->serverbuf.getStringA(0x10);
							teamComment = client->serverbuf.getStringA(0x28);
							teamSurvivalWin = client->serverbuf.get<uint32_t>();
							teamSurvivalLose = client->serverbuf.get<uint32_t>();
							teamCount = client->serverbuf.get<uint32_t>();
							teamFlag = client->serverbuf.get<uint32_t>();

							for (uint32_t i = 0; i < teamCount; i++)
							{
								client->teammembers[i].license = client->serverbuf.get<uint32_t>();
								client->serverbuf.getArray((uint8_t*)&client->teammembers[i].name[0], 0x10);
								client->teammembers[i].rank = client->serverbuf.get<uint32_t>();
								client->teammembers[i].leader = client->serverbuf.get<uint8_t>();
								client->teammembers[i].area = client->serverbuf.get<uint8_t>();
							}
						}
						teamarea = client->serverbuf.get<uint8_t>();
						client->teamareaaccess = teamarea;

						// If DB had cars but garagecount was 0/NULL (manual inserts), repair count
						// before getCarCount() — it only scans garageCount*4 bays.
						if (client->garagedata.garageCount == 0 && loadedCarCount > 0)
						{
							client->garagedata.garageType.clear();
							client->garagedata.garageType.push_back(0);
							client->garagedata.garageCount = 1;
						}
					}

					// Manually-created DB accounts often have a handle but no garage/cars.
					// Sending 0x0182 with "garage exists" and 0 bays crashes the 2.03 client.
					uint32_t occupiedBays = 0;
					for (auto& car : client->garagedata.car)
						if (car.carID != 0xFFFFFFFF) occupiedBays++;
					bool hasPlayableGarage = (handle != "" && occupiedBays > 0);
					if (!hasPlayableGarage)
					{ // Create new handle / first-time setup
						if (handle != "")
						{
							client->logger->Log(Logger::LOGTYPE_CLIENT,
								L"License %u has handle '%s' but no cars - falling back to character-creation 0x0182.",
								license, client->logger->toWide(handle).c_str());
							client->handle.clear();
						}
						client->packetEnable(0x0E);
						client->outbuf.clearBuffer();
						client->outbuf.setSize(0x06);
						client->outbuf.setOffset(0x06);
						client->outbuf.setType(0x100);
						client->outbuf.setSubType(0x182);
						client->outbuf.append<uint8_t>(0); // Players garage exists
						client->outbuf.append<uint8_t>(0); // if 2 additional int below. If 0 Team Center and Tuned Car Exchange is unavailable
						client->outbuf.append<uint32_t>(0);
						client->outbuf.append<uint32_t>(0);
						client->initializeTeam();
						client->outbuf.appendArray((uint8_t*)&client->teamdata, sizeof(client->teamdata));

						client->outbuf.append<uint16_t>((client->privileges & 1) | 0x42); // Player Flag
						client->outbuf.append<uint8_t>(4); // Bays/Cars (set 4 to initialize all bays)
						client->outbuf.append<uint32_t>(client->careerdata.ranking); // Ranking number
						client->outbuf.append<uint8_t>(0); // No subgarages on new accounts

						client->outbuf.append<uint32_t>(timeGetTime()); // Time Played
						client->outbuf.append<uint16_t>(0x24); // Course Section
						client->outbuf.append<uint16_t>(0x03);
						client->outbuf.append<uint16_t>(0);
						client->outbuf.append<uint16_t>(0);
						client->outbuf.append<uint8_t>(client->currentCourse);
					}
					else
					{ // Load Existing Account
						client->canSave = true;

						// Ensure garage metadata is valid before building 0x0182
						if (client->garagedata.garageCount == 0 || client->garagedata.garageType.empty())
						{
							client->garagedata.garageType.clear();
							client->garagedata.garageType.push_back(0);
							client->garagedata.garageCount = 1;
						}
						if (client->garagedata.garageCount > 2) client->garagedata.garageCount = 2;
						while (client->garagedata.garageType.size() < client->garagedata.garageCount)
							client->garagedata.garageType.push_back(0);

						if (activeCarBay < 0 || activeCarBay >= (int8_t)(client->garagedata.garageCount * 4) ||
							client->garagedata.car[activeCarBay].carID == 0xFFFFFFFF)
						{
							// Pick first occupied bay
							activeCarBay = 0;
							for (uint32_t i = 0; i < client->garagedata.car.size(); i++)
							{
								if (client->garagedata.car[i].carID != 0xFFFFFFFF)
								{
									activeCarBay = (int8_t)i;
									break;
								}
							}
						}

						client->outbuf.clearBuffer();
						client->outbuf.setSize(0x06);
						client->outbuf.setOffset(0x06);
						client->outbuf.setType(0x100);
						client->outbuf.setSubType(0x182);
						client->outbuf.append<uint8_t>(1); // Players garage exists
						client->outbuf.appendString(client->handle, 0x10);
						client->outbuf.append<uint32_t>(activeCarBay + 1); // Active Car Slot
						client->outbuf.append<uint8_t>(0); // 0: Disable Team Center and Tuned Car Exchange
						client->outbuf.append<uint32_t>(0);
						client->outbuf.append<uint32_t>(0);
						// Team Data
						if (!inTeam)
							client->initializeTeam();
						else
						{
							client->teamdata.teamID = teamID;
							strncpy(&client->teamdata.name[0], teamName.c_str(), 0x10);
							strncpy(&client->teamdata.leaderName[0], teamLeaderName.c_str(), 0x10);
							strncpy(&client->teamdata.comment[0], teamComment.c_str(), 0x28);
							client->teamdata.memberCount = (uint8_t)min(teamCount, 255u);
							client->teamdata.survivalWins = teamSurvivalWin;
							client->teamdata.survivalLoses = teamSurvivalLose;
							client->teamdata.inviteOnly = teamFlag & 0x01 ? 1 : 0;
							client->teamdata.unknown4 = 0;
							client->teamdata.unknown5 = 0;
						}
						client->outbuf.appendArray((uint8_t *)&client->teamdata, sizeof(client->teamdata));

						client->outbuf.append<uint16_t>((client->privileges & 1) | 0x42); // Player flag
						client->outbuf.append<uint8_t>((uint8_t)(min(1, (int)client->garagedata.garageCount) * 4)); // Car/Bay Count
						client->outbuf.append<uint32_t>(client->careerdata.ranking); // Ranking number
						client->outbuf.append<uint8_t>(client->garagedata.garageCount);
						for (int8_t i = 0; i < client->garagedata.garageCount; i++)
						{
							client->outbuf.append<uint32_t>(i * 2); // Garage number 0: main, 1: second
							client->outbuf.append<uint32_t>(client->garagedata.garageType[i]); // Garage Type
						}

						client->outbuf.append<uint32_t>(timeGetTime()); // Time Played
						srand(timeGetTime());
						uint32_t rng = ((rand() & 0xFF) << 24) | ((rand() & 0xFF) << 16) | ((rand() & 0xFF) << 8) | (rand() & 0xFF);
						uint32_t startCount = (uint32_t)(sizeof(startPositions) / sizeof(startPositions[0]));
						client->startJunction = startPositions[rng % startCount][0];
						client->startDistance = startPositions[rng % startCount][1];
						client->outbuf.append<uint16_t>(client->notBeginner ? client->startJunction : 0xffff);
						client->outbuf.append<uint16_t>(client->notBeginner ? client->startDistance : 0xffff);
						client->outbuf.append<uint16_t>(0xffff);
						client->outbuf.append<uint16_t>(client->notBeginner ? 0xffff : 0);
						if (client->notBeginner == false) client->currentCourse = 0x15;
						client->outbuf.append<uint8_t>(client->currentCourse);

						client->setActiveCar(activeCarBay);
					}
				}
			}
			client->Send();
		}
	}
	break;
	case 0x0001: // Handle check
	{
		int32_t clientSocket = client->serverbuf.get<int32_t>(0x06);
		uint8_t result = client->serverbuf.get<uint8_t>(0x0A);

		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0xE00);
		client->outbuf.setSubType(0xE80);
		client->outbuf.append<uint8_t>(result ? 1 : 0); // 0x00 OK, 0x01 Taken
		client->Send();
	}
	break;
	}
}
