#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

typedef struct {
    float Kp, Ki, Kd, tau;
    float limMin, limMax;
    // ... (otros parámetros)
} PIDController;

void PID_Init(PIDController *pid);
float PID_Update(PIDController *pid, float setpoint, float measurement, float dt);
void PID_Reset(PIDController *pid); // Opcional: resetear estado interno

#endif // PID_CONTROLLER_H
