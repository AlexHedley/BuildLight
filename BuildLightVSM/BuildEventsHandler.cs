﻿using System;
using System.Threading;
using MonoDevelop.Core;
using MonoDevelop.Ide;
using MonoDevelop.Projects;

using BuildLight.Common;

namespace BuildLightVSM
{
    public class BuildEventsHandler
    {
        readonly DeviceList deviceList = DeviceList.Shared;

        CancellationTokenSource? startCancellationTokenSource = null;
        CancellationTokenSource? endCancellationTokenSource = null;
        bool cummulativeBuildSuccess = false;

        public BuildEventsHandler()
        {
            IdeApp.Initialized += HandleInitialized;
        }

        private void HandleInitialized(object sender, EventArgs e)
        {
            IdeApp.ProjectOperations.StartBuild += HandleStartBuild;
            IdeApp.ProjectOperations.EndBuild += HandleEndBuild;
            deviceList.RefreshAsync().ContinueWith(t =>
            {
                if (t.IsFaulted)
                    Console.WriteLine(t.Exception);
            });
        }

        private async void HandleStartBuild(object sender, BuildEventArgs args)
        {            
            try
            {
                startCancellationTokenSource?.Cancel();
                endCancellationTokenSource?.Cancel();
                startCancellationTokenSource = new CancellationTokenSource();
                cummulativeBuildSuccess = true;
                await deviceList.SetColorAsync(0, 0, 255, startCancellationTokenSource.Token);
            }
            catch (Exception ex)
            {
                LoggingService.LogError("Failed to handle start build", ex);
            }
        }

        private async void HandleEndBuild(object sender, BuildEventArgs args)
        {
            try
            {
                startCancellationTokenSource?.Cancel();
                endCancellationTokenSource?.Cancel();
                endCancellationTokenSource = new CancellationTokenSource();

                var buildStatus = args.Success;
                cummulativeBuildSuccess = cummulativeBuildSuccess && buildStatus;

                if (cummulativeBuildSuccess)
                {
                    await deviceList.SetColorAsync(red: 0, green: 255, 0, endCancellationTokenSource.Token);
                }
                else
                {
                    await deviceList.SetColorAsync(red: 255, green: 0, 0, endCancellationTokenSource.Token);
                }
            }
            catch (Exception ex)
            {
                LoggingService.LogError("Failed to handle end build", ex);
            }
        }
    }
}
