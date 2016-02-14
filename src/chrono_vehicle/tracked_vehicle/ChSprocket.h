// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2014 projectchrono.org
// All right reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Radu Serban
// =============================================================================
//
// Base class for a tracked vehicle sprocket. A sprocket is responsible for
// contact processing with the track shoes of the containing track assembly.
//
// A derived class which implements a particular sprocket template must specify
// the custom collision callback object and provide the gear profile as a 2D
// path. The gear profile, a ChLinePath geometric object, is made up of an
// arbitrary number of sub-paths of type ChLineArc or ChLineSegment sub-lines.
// These must be added in  clockwise order, and the end of sub-path i must be
// coincident with beginning of sub-path i+1.
//
// The reference frame for a vehicle follows the ISO standard: Z-axis up, X-axis
// pointing forward, and Y-axis towards the left of the vehicle.
//
// =============================================================================

#ifndef CH_SPROCKET_H
#define CH_SPROCKET_H

#include <vector>

#include "chrono/physics/ChSystem.h"
#include "chrono/physics/ChBody.h"
#include "chrono/physics/ChBodyAuxRef.h"
#include "chrono/physics/ChLinkLock.h"
#include "chrono/physics/ChShaft.h"
#include "chrono/physics/ChShaftsBody.h"
#include "chrono/geometry/ChCLinePath.h"
#include "chrono/geometry/ChCLineSegment.h"
#include "chrono/geometry/ChCLineArc.h"

#include "chrono_vehicle/ChApiVehicle.h"
#include "chrono_vehicle/ChSubsysDefs.h"

namespace chrono {
namespace vehicle {

// Forward declaration
class ChTrackAssembly;

///
///
///
class CH_VEHICLE_API ChSprocket {
  public:
    ChSprocket(const std::string& name  ///< [in] name of the subsystem
               );

    virtual ~ChSprocket();

    /// Get the name identifier for this sprocket subsystem.
    const std::string& GetName() const { return m_name; }

    /// Set the name identifier for this sprocket subsystem.
    void SetName(const std::string& name) { m_name = name; }

    /// Get the number of teeth of the gear.
    virtual int GetNumTeeth() const = 0;

    /// Get the track assembly radius.
    /// This quantity is used during the automatic track assembly. It represents a
    /// safe distance from the sprocket gear center at which the track shoes can be
    /// instantiated.
    virtual double GetAssemblyRadius() const = 0;

    /// Get a handle to the gear body.
    ChSharedPtr<ChBody> GetGearBody() const { return m_gear; }

    /// Get a handle to the axle shaft.
    ChSharedPtr<ChShaft> GetAxle() const { return m_axle; }

    /// Get a handle to the revolute joint.
    ChSharedPtr<ChLinkLockRevolute> GetRevolute() const { return m_revolute; }

    /// Get the angular speed of the axle.
    double GetAxleSpeed() const { return m_axle->GetPos_dt(); }

    /// Set contact material properties.
    /// This function must be called before Initialize().
    void SetContactMaterial(float friction_coefficient,     ///< [in] coefficient of friction
                            float restitution_coefficient,  ///< [in] coefficient of restitution
                            float young_modulus,            ///< [in] Young's modulus of elasticity
                            float poisson_ratio             ///< [in] Poisson ratio
                            );

    /// Turn on/off collision flag for the gear wheel.
    void SetCollide(bool val) { m_gear->SetCollide(val); }

    /// Initialize this sprocket subsystem.
    /// The sprocket subsystem is initialized by attaching it to the specified
    /// chassis body at the specified location (with respect to and expressed in
    /// the reference frame of the chassis).
    void Initialize(ChSharedPtr<ChBodyAuxRef> chassis,  ///< [in] handle to the chassis body
                    const ChVector<>& location,         ///< [in] location relative to the chassis frame
                    ChTrackAssembly* track              ///< [in] pointer to containing track assembly
                    );

    /// Apply the provided motor torque.
    /// The given torque is applied to the axle. This function provides the interface
    /// to the drivetrain subsystem (intermediated by the vehicle system).
    void ApplyAxleTorque(double torque  ///< [in] value of applied torque
                         );

    /// Log current constraint violations.
    void LogConstraintViolations();

  protected:
    /// Return the mass of the gear body.
    virtual double GetGearMass() const = 0;

    /// Return the moments of inertia of the gear body.
    virtual const ChVector<>& GetGearInertia() = 0;

    /// Return the inertia of the axle shaft.
    virtual double GetAxleInertia() const = 0;

    /// Return the distance between the two gear profiles.
    virtual double GetSeparation() const = 0;

    /// Return the 2D gear profile.
    /// The gear profile, a ChLinePath geometric object, is made up of an arbitrary number
    /// of sub-paths of type ChLineArc or ChLineSegment sub-lines. These must be added in
    /// clockwise order, and the end of sub-path i must be coincident with beginning of
    /// sub-path i+1.
    virtual ChSharedPtr<geometry::ChLinePath> GetProfile() = 0;

    /// Return the custom collision callback object.
    /// Note that the derived need not delete this object (it is deleted in the
    /// destructor of this base class).
    virtual ChSystem::ChCustomComputeCollisionCallback* GetCollisionCallback(
        ChTrackAssembly* track  ///< [in] pointer to containing track assembly
        ) = 0;

    /// Add visualization of the gear wheel.
    /// The default implementation renders the gear tooth profiles as a line path.
    virtual void AddGearVisualization();

    std::string m_name;                           ///< name of the subsystem
    ChSharedPtr<ChBody> m_gear;                   ///< handle to the sprocket gear body
    ChSharedPtr<ChShaft> m_axle;                  ///< handle to gear shafts
    ChSharedPtr<ChShaftsBody> m_axle_to_spindle;  ///< handle to gear-shaft connector
    ChSharedPtr<ChLinkLockRevolute> m_revolute;   ///< handle to sprocket revolute joint

    ChSystem::ChCustomComputeCollisionCallback* m_callback;  ///< custom collision functor object

    float m_friction;
    float m_restitution;
    float m_young_modulus;
    float m_poisson_ratio;
};

/// Vector of handles to sprocket subsystems.
typedef std::vector<ChSharedPtr<ChSprocket> > ChSprocketList;

}  // end namespace vehicle
}  // end namespace chrono

#endif
