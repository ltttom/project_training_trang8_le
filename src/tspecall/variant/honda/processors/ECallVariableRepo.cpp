/**
 * @file        ECallVariableRepo.cpp
 * @author      eunbi.lee@lge.com
 * @version     1.0.00
*/
#include "ECallVariableRepo.h"

namespace ecallapp {

std::mutex ECallVariableRepo::mMutex;

ECallVariableRepo::ECallVariableRepo() :
    mECallType(ecall_type::UNDEFINED),
    mIgAccCondition(false),
    mAccCondition(false),
    mIgCondition(false),
    mCurrentFeatureName(""),
    mIsAfterRecovery(false),
    mECallClearAudioState(0) {
}

ECallVariableRepo& ECallVariableRepo::getInstance() {
    static ECallVariableRepo instance;
    return instance;
}

void ECallVariableRepo::setECallType(int32_t ecallType) {
    std::lock_guard<std::mutex> lock(mMutex);
    mECallType = ecallType;
}

int32_t ECallVariableRepo::getECallType() const {
    std::lock_guard<std::mutex> lock(mMutex);
    return mECallType;
}

void ECallVariableRepo::updateIgAccCondition() {
    std::lock_guard<std::mutex> lock(mMutex);
    auto powerManager = PowerManagerAdapter::getInstance();
    if (powerManager != nullptr) {
        mAccCondition = powerManager->getACCStatus() == MCU_VALUE_STATUS::MCU_STATUS_ON;
        mIgCondition = powerManager->isIgnitionOn();
        mIgAccCondition = mAccCondition && mIgCondition;
    } else {
        LOG_E(DLT_STRING("PowerManagerAdapter instance is null"));
        mAccCondition = false;
        mIgCondition = false;
        mIgAccCondition = false;
    }
}

bool ECallVariableRepo::getIgCondition() const {
    std::lock_guard<std::mutex> lock(mMutex);
    return mIgCondition;
}

bool ECallVariableRepo::getAccCondition() const {
    std::lock_guard<std::mutex> lock(mMutex);
    return mAccCondition;
}

bool ECallVariableRepo::getIgAccCondition() const {
    std::lock_guard<std::mutex> lock(mMutex);
    return mIgAccCondition;
}

void ECallVariableRepo::setECallClearAudioState(int32_t state) {
    std::lock_guard<std::mutex> lock(mMutex);
    mECallClearAudioState = state;
}

int32_t ECallVariableRepo::getECallClearAudioState() const {
    std::lock_guard<std::mutex> lock(mMutex);
    return mECallClearAudioState;
}

bool ECallVariableRepo::getProtectionMode() {
    std::lock_guard<std::mutex> lock(mMutex);
    EOperationMode op_mode;
    if (OperationModeManager::instance()->getOperationMode(op_mode) == E_OK) {
        std::string operationMode = op_mode.getStringEnum();
        if (operationMode.compare("EOperationMode::OEMFactoryMode_ProtectionMode") == 0) {
            LOG_I(DLT_STRING("TSU is in Protection mode, do nothing"));
            return true;
        } else {
            LOG_I(DLT_STRING("TSU is in Normal mode"));
            return false;
        }
    }
    // If TSU cannot get the operation mode, we assume it is in Normal mode
    LOG_E(DLT_STRING("Failed to get operation mode, assuming TSU is in Normal mode"));
    return false;
}

} // namespace ecallapp
