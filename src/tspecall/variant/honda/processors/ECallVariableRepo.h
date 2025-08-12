/**
 * @file        ECallVariableRepo.h
 * @author      eunbi.lee@lge.com
 * @version     1.0.00
*/

#ifndef ECALL_VARIABLE_REPO_H
#define ECALL_VARIABLE_REPO_H

#include <mutex>
#include <string>
#include <services/OperationModeManagerService/OperationModeManager.h>
#include "../../../core/services/PowerManagerAdapter.h"
#include "InternalParamsDef.h"

namespace ecallapp {

/**
 * This class stores various state variables used by the eCall service
 * and provides getter/setter methods to access them from different components.
 */
class ECallVariableRepo {
public:
    /**
     * @brief Get the singleton instance of ECallVariableRepo
     *
     * @return Reference to the singleton instance
     */
    static ECallVariableRepo& getInstance();

    /**
     * @brief Set the eCall type
     *
     * @param ecallType The eCall type to set
     */
    void setECallType(int32_t ecallType);

    /**
     * @brief Get the current eCall type
     *
     * @return The current eCall type
     */
    int32_t getECallType() const;

    /**
     * @brief Update the ignition and ACC condition state by checking PowerManagerAdapter
     *
     * This method checks the current ignition and ACC status from PowerManagerAdapter
     * and updates the internal state variable accordingly.
     */
    void updateIgAccCondition();

    /**
     * @brief Get the current ignition
     *
     * @return true if ignition is on
     */
    bool getIgCondition() const;

    /**
     * @brief Get the current ACC
     *
     * @return true if ACC is on
     */
    bool getAccCondition() const;

    /**
     * @brief Get the current ignition and ACC condition
     *
     * @return true if both ignition is on and ACC is on, false otherwise
     */
    bool getIgAccCondition() const;

    /**
     * @brief Set the eCall clear audio state
     *
     * @param state The audio state to set
     */
    void setECallClearAudioState(int32_t state);

    /**
     * @brief Get the eCall clear audio state
     *
     * @return The current audio state
     */
    int32_t getECallClearAudioState() const;

    /**
     * @brief Check if the TSU is in Protection mode
     *
     * @return true if the TSU is in Protection mode, false TSU is in normal mode
     */
    bool getProtectionMode();

private:
    // Private constructor to enforce singleton pattern
    ECallVariableRepo();

    // Delete copy constructor and assignment operator
    ECallVariableRepo(const ECallVariableRepo&) = delete;
    ECallVariableRepo& operator=(const ECallVariableRepo&) = delete;

    // Mutex for thread safety
    static std::mutex mMutex;

    // State variables
    int32_t mECallType;
    bool mIgAccCondition;
    bool mAccCondition;
    bool mIgCondition;
    std::string mCurrentFeatureName;
    bool mIsAfterRecovery;
    int32_t mECallClearAudioState;
};

} // namespace ecallapp

#endif // ECALL_VARIABLE_REPO_H
